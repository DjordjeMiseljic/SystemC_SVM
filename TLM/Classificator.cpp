#ifndef CLASSIFICATOR_C
#define CLASSIFICATOR_C
#include "Classificator.hpp"

#define P_CHECK_OVERFLOW if(p.overflow_flag()) cout<<BKG_YELLOW<<BLACK<<"WARNING"<<BKG_RST<<D_YELLOW<<" OVERFLOW DETECTED IN CORE"<<RST<<endl;
#define R_CHECK_OVERFLOW if(res_v[core].overflow_flag()) cout<<BKG_YELLOW<<BLACK<<"WARNING"<<BKG_RST<<D_YELLOW<<" OVERFLOW DETECTED IN CORE"<<RST<<endl;
#define A_CHECK_OVERFLOW if(acc.overflow_flag()) cout<<BKG_YELLOW<<BLACK<<"WARNING"<<BKG_RST<<D_YELLOW<<" OVERFLOW DETECTED IN CORE"<<RST<<endl;

#define SV_LEN 784
int sum_of_sv(int to_element)
{
   if(to_element == -1)
      return 0;
   int sum = 0;
   for(int i=0;i<=to_element;i++ )
      sum += sv_array[i];
   
   return sum;
}
#define SV0 0
#define SV1 (466*(SV_LEN+1))+1
#define SV2 SV1+(408*(SV_LEN+1))
#define SV3 SV2+(827*(SV_LEN+1))
#define SV4 SV3+(948*(SV_LEN+1))
#define SV5 SV4+(683*(SV_LEN+1))
#define SV6 SV5+(756*(SV_LEN+1))
#define SV7 SV6+(533*(SV_LEN+1))
#define SV8 SV7+(659*(SV_LEN+1))
#define SV9 SV8+(1078*(SV_LEN+1))

const array<int, 10> sv_start_addr = {SV0, SV1, SV2, SV3, SV4, SV5, SV6, SV7, SV8, SV9};

Classificator::Classificator(sc_module_name name): sc_module(name),
                                                   tsoc("classificator_tsoc"),
                                                   isoc("classificator_isoc"),
                                                   dmi_valid(false)
{
   tsoc(*this);
   isoc(*this);
   file_extract();
   cout<<name<<" constucted"<<endl;
   image_v.reserve(SV_LEN);
   res_v.reserve(10);
   for (int i = 0; i < 10; ++i)
   {
      cout<<sv_start_addr[i]<<endl;
   }

}



void Classificator::b_transport(pl_t& pl, sc_time& offset)
{
	
   tlm_command cmd    = pl.get_command();
   uint64 adr         = pl.get_address();
   unsigned char *buf = pl.get_data_ptr();
   unsigned int len   = pl.get_data_length();

   if(adr!=1)
   {
      SC_REPORT_ERROR("Classificator"," Not my payload ");
      return;
   }

   switch(cmd)
   {

   case TLM_WRITE_COMMAND://--------------------------------------------------------CLASSIFY IMAGE
         
      image_v.clear();
      for(unsigned int i=0; i<len; i++)
         image_v.push_back(((din_t *)buf)[i]);

      // for(unsigned int i=0; i<len; i++)
      //    cout<< image_v[i]<<"\t";
      //    cout<<endl;

      offset = SC_ZERO_TIME;
         
      //This is used only for debugging
      /*
        unsigned char dbg_buf[10000];
        tlm_generic_payload dbg_pl;
        dbg_pl.set_data_ptr(dbg_buf);
        dbg_pl.set_command(TLM_READ_COMMAND);
      */
#ifdef QUANTUM
      tlm_utils::tlm_quantumkeeper qk;
      qk.reset();
#endif
        
      cmd=TLM_READ_COMMAND;
      pl.set_command         ( cmd );
      pl.set_data_ptr        ( buf );

      for(unsigned int core=0; core<10; core++)
      {
         acc=0;

         for( int sv=0; sv<sv_array[core]; sv++)
         {
#ifdef QUANTUM
            qk.inc(sc_time(4, SC_NS));
            offset = qk.get_local_time();
#else
            offset += sc_time(4, SC_NS);
#endif
               
            //REQUEST SV
            adr=sv_start_addr[core]+sv*SV_LEN;
            /*
              address where support vectors for current core start +
              current sv * number of pixels 
            */

            pl.set_address ((uint64)adr);
            len=SV_LEN;
            pl.set_data_length (len);
            pl.set_response_status ( TLM_INCOMPLETE_RESPONSE );
            isoc->b_transport(pl,offset);
            //#######################################
            //FOR TESTING IF CORRECT SV IS RECEIVED
            din_t* test = (din_t*)pl.get_data_ptr();
            for (int i = 0; i < 784; ++i)
               if(test[i]!=sv_c[core][sv*784+i])
               {
                  cout<<RED<<"ERROR MISMATCH"<<RST<<endl;
                  cout<<"core: "<<core<<", sv: "<<sv<<", adr: "<<adr<<endl;
                  return;
               }
            //########################################
            if (pl.get_response_status() != TLM_OK_RESPONSE)
               SC_REPORT_INFO("Classificator","WARNING: WRONG RESPONSE");
               
            buf=pl.get_data_ptr();

            p=1.0;
            for( int i=0; i<SV_LEN; i++)
            {
               p+=image_v[i]*((din_t*)buf)[i];
               P_CHECK_OVERFLOW
                  }
            //.cout<<p<<"";
            p*=0.1;
            P_CHECK_OVERFLOW

               p=p*p*p;
            P_CHECK_OVERFLOW
               
               //REQUEST LAMBDA
               if(core == 0)
                  adr=sv_start_addr[core]+sv_array[core]*SV_LEN+sv;
               else
                  adr=sv_start_addr[core]-1+sv_array[core]*SV_LEN+sv;
                  /*
              address where support vectors for current core start + 
              num of sv for current core * num of pixels +
              offset for current sv 
            */
            pl.set_address (adr);
            len=1;
            pl.set_data_length (len);
            pl.set_response_status ( TLM_INCOMPLETE_RESPONSE );
            isoc->b_transport(pl,offset);
            //###############################################
            //FOR TESTING IF CORRECT LAMBDA IS RECEIVED
            lin_t *test_l = (lin_t*)pl.get_data_ptr();
            if(*test_l!=lambdas[core][sv])
            {
               cout<<GREEN<<"LAMBDA MISMATCH"<<RST<<endl;
               cout<<"core: "<<core<<", sv: "<<sv<<", adr: "<<adr<<endl;
               return;
            }
            //#################################################
            if (pl.get_response_status() != TLM_OK_RESPONSE)
               SC_REPORT_INFO("Classificator","WARNING: WRONG RESPONSE");
               
            buf=pl.get_data_ptr();

            p*= *(lin_t*)buf;
            P_CHECK_OVERFLOW
               
               acc+=p;
            A_CHECK_OVERFLOW
               }

         //REQUEST BIAS
         if(core == 0)
            adr=sv_start_addr[core]+sv_array[core]*(SV_LEN+1);
         else
            adr=sv_start_addr[core]-1+sv_array[core]*(SV_LEN+1);
         /*
           address where support vectors for current core start + 
           number of support vectors for current core * 
           (num of pixels + 1 for lambdas)
         */
         pl.set_address (adr);
         len=1;
         pl.set_data_length (len);
         pl.set_response_status ( TLM_INCOMPLETE_RESPONSE );
         isoc->b_transport(pl,offset);
         //################################################
         //FOR TESTING IF CORRECT BIAS IS RECEIVED
         bin_t *test_b = (bin_t*)pl.get_data_ptr();
         if(*test_b!=biases[core])
         {
            cout<<YELLOW<<"BIAS MISMATCH"<<RST<<endl;
            cout<<"core: "<<core<<", adr: "<<adr<<endl;
            return;
         }
         //################################################
         if (pl.get_response_status() != TLM_OK_RESPONSE)
            SC_REPORT_INFO("Classificator","WARNING: WRONG RESPONSE");

         buf=pl.get_data_ptr();
         acc+=*(bin_t*)buf;
         A_CHECK_OVERFLOW

            res_v.push_back (acc);
         R_CHECK_OVERFLOW
         
            }
      // cout<<"RESULTS:"<<endl;
      // for(int i=0; i<10; i++)
      //    cout<<res_v[i]<<" ";
      //get classified number from res
      max_res=res_v[0];
      cl_num=0;
      for(int i=1; i<10; i++)
      {
         //cout<<"res of "<<i<<"is: "<<res_v[i]<<endl;
         if(max_res<res_v[i])
         {
            max_res=res_v[i];
            cl_num=(num_t)i;
            //cout<<cl_num<<endl;
         }
      }

      pl.set_response_status( TLM_OK_RESPONSE );
      break;

   case TLM_READ_COMMAND://--------------------------------------------------------RETURN RESULTS
         
      buf=(unsigned char *)&cl_num;
      pl.set_data_ptr        ( buf );
      pl.set_response_status( TLM_OK_RESPONSE );
      break;

   default:
      pl.set_response_status( TLM_COMMAND_ERROR_RESPONSE );
   }

}


tlm_sync_enum Classificator::nb_transport_fw(pl_t& pl, phase_t& phase, sc_time& offset)
{
   return TLM_ACCEPTED;
}

bool Classificator::get_direct_mem_ptr(pl_t& pl, tlm_dmi& dmi)
{
   dmi.allow_read_write();

   return true;
}

unsigned int Classificator::transport_dbg(pl_t& pl)
{

   return 0;
}

tlm_sync_enum Classificator::nb_transport_bw(pl_t& pl, phase_t& phase, sc_time& offset)
{
   return TLM_ACCEPTED;
}

void Classificator::invalidate_direct_mem_ptr(uint64 start, uint64 end)
{
   dmi_valid = false;
}
void Classificator::file_extract()
{
   
   string sv_line;
   string t_line;
   string l_line;
   string b_line;
   string str;
   int lines;
   int j, k=0;
   int sv_len = 784;
   int sum = 0;
   
   for (int i = 0; i < 10; ++i)
   {
      sv_c[i].reserve(sv_array[i]*784);
      lambdas[i].reserve(sv_array[i]);
   }

   for(int i=0; i<10; i++)
   {
      
      //extracting biases
      str = "../../ML_number_recognition_SVM/saved_data/bias/bias";
      str = str + to_string(i);
      str = str+".txt";
      ifstream b_file(str);
      getline(b_file,b_line);
      biases[i] = stod(b_line);
      //extracting support vectors
      str = "../../ML_number_recognition_SVM/saved_data/support_vectors/sv";
      str = str + to_string(i);
      str = str+".txt";
      lines = num_of_lines(str);
      sv_lines[i] = lines;
      sum += lines;
      //cout<<"sv "<<i<<"is "<<sv_lines[i]<<endl;
      ifstream sv_file(str);
      if(sv_file.is_open())
         while(j!=lines*sv_len)
         {

            if(k == sv_len-1)
            {
               getline(sv_file, sv_line, '\n');
               k = 0;
            }
            else
            {
               getline(sv_file, sv_line, ' ');
               k++;
            }
            sv_c[i].push_back(stod(sv_line));
            j++;
         }
      else
         cout<<RED<<"ERROR OPENING SV_FILE number: "<<i<<RST<<endl;
      sv_file.close();
      j = 0;
      k = 0;
      //extracting lambdas and targets
      lines = num_of_lines(str);
      str = "../../ML_number_recognition_SVM/saved_data/lambdas/lambdas";
      str = str + to_string(i);
      str = str+".txt";
      ifstream l_file(str);

      str = "../../ML_number_recognition_SVM/saved_data/targets/targets";
      str = str + to_string(i);
      str = str+".txt";
      ifstream t_file(str);
      if(t_file.is_open() && l_file.is_open())
         while(j != lines)
         {
            //extracting lambda
            getline(l_file,l_line);
            //extracting target
            getline(t_file,t_line);
            lambdas[i].push_back(stod(t_line)*1000*stod(l_line));
            j++;
         }
      else
         cout<<RED<<"ERROR OPENING L_FILE of T_FILE number: "<<i<<RST<<endl;
      j = 0;
      l_file.close();
      t_file.close();
      b_file.close();
   }
   cout<<"sum is: "<<sum<<endl;

}
int Classificator::num_of_lines(string str)
{
   int count = 0;
   string line;
   ifstream str_file(str);
   if(str_file.is_open())

   {
      while(getline(str_file,line))
         count++;
      str_file.close();
   }
   else
      cout<<"error opening str file in method num of lines"<<endl;
   return count;
   
}

#endif
