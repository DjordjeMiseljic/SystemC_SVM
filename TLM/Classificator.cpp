#ifndef CLASSIFICATOR_C
#define CLASSIFICATOR_C
#include "Classificator.hpp"

#define P_CHECK_OVERFLOW if(p.overflow_flag()) cout<<BKG_YELLOW<<BLACK<<"WARNING"<<BKG_RST<<D_YELLOW<<" OVERFLOW DETECTED IN CORE"<<RST<<endl;
#define R_CHECK_OVERFLOW if(res_v[core].overflow_flag()) cout<<BKG_YELLOW<<BLACK<<"WARNING"<<BKG_RST<<D_YELLOW<<" OVERFLOW DETECTED IN CORE"<<RST<<endl;
#define A_CHECK_OVERFLOW if(acc.overflow_flag()) cout<<BKG_YELLOW<<BLACK<<"WARNING"<<BKG_RST<<D_YELLOW<<" OVERFLOW DETECTED IN CORE"<<RST<<endl;


#define SV0 0
#define SV1 (361*(SV_LEN+1))+1
#define SV2 SV1+(267*(SV_LEN+1))+1
#define SV3 SV2+(581*(SV_LEN+1))+1
#define SV4 SV3+(632*(SV_LEN+1))+1
#define SV5 SV4+(480*(SV_LEN+1))+1
#define SV6 SV5+(513*(SV_LEN+1))+1
#define SV7 SV6+(376*(SV_LEN+1))+1
#define SV8 SV7+(432*(SV_LEN+1))+1
#define SV9 SV8+(751*(SV_LEN+1))+1

const array<int, 10> sv_start_addr = {SV0, SV1, SV2, SV3, SV4, SV5, SV6, SV7, SV8, SV9};

Classificator::Classificator(sc_module_name name): sc_module(name)
{
   s_cl_t.register_b_transport(this, &Classificator::b_transport);
   cout<<name<<" constucted"<<endl;
   SC_THREAD(classify);
   
   image_v.reserve(SV_LEN);
   p_exp.bind(s_new);
   res_v.reserve(10);
   toggle = SC_LOGIC_0;
}


void Classificator::classify ()
{
   din_t fifo_tmp;
   bool nbf=true; 
   wait(1,SC_NS);
   toggle = (toggle==SC_LOGIC_0)? SC_LOGIC_1 : SC_LOGIC_0; 
   s_new.write(toggle);//demand new, send interrupt                 **
   while(1)
   {
      wait(e_start);
      res_v.clear();
      for(unsigned int core=0; core<10; core++)
      {
         acc=0;

         for( int sv=0; sv<sv_array[core]; sv++)
         {

            toggle = (toggle==SC_LOGIC_0)? SC_LOGIC_1 : SC_LOGIC_0; 
            s_new.write(toggle);//demand new, send interrupt                 **

            p=1.0;
            for( int i=0; i<SV_LEN; i++)
            {
               p_fifo->read(fifo_tmp);
               p+=image_v[i]*fifo_tmp;
               P_CHECK_OVERFLOW
            }
            p*=0.1;
            P_CHECK_OVERFLOW

            p=p*p*p;
            P_CHECK_OVERFLOW
               
            toggle = (toggle==SC_LOGIC_0)? SC_LOGIC_1 : SC_LOGIC_0; 
            s_new.write(toggle);//demand new, send interrupt                 **

            p_fifo->read(fifo_tmp);
            p*= fifo_tmp;
            P_CHECK_OVERFLOW
               
            acc+=p;
            A_CHECK_OVERFLOW
         }
         toggle = (toggle==SC_LOGIC_0)? SC_LOGIC_1 : SC_LOGIC_0; 
         s_new.write(toggle);//demand new, send interrupt                 **

         p_fifo->read(fifo_tmp);
         acc+=fifo_tmp;
         A_CHECK_OVERFLOW

         res_v.push_back (acc);
         R_CHECK_OVERFLOW
         
      }

      max_res=res_v[0];
      cl_num=0;
      for(int i=1; i<10; i++)
      {
         if(max_res<res_v[i])
         {
            max_res=res_v[i];
            cl_num=(num_t)i;
         }
      }

      toggle = (toggle==SC_LOGIC_0)? SC_LOGIC_1 : SC_LOGIC_0; 
      s_new.write(toggle);//demand new, send interrupt                 **
   }
}
void Classificator::b_transport(pl_t& pl, sc_time& offset)
{
	
   tlm_command cmd    = pl.get_command();
   uint64 adr         = pl.get_address();
   unsigned char *buf = pl.get_data_ptr();
   unsigned int len   = pl.get_data_length();

   switch(cmd)
   {
   case TLM_WRITE_COMMAND://--------------SAVE NEW PICTURE
         
      image_v.clear();
      for(unsigned int i=0; i<len; i++)
         image_v.push_back(((din_t *)buf)[i]);
      pl.set_response_status( TLM_OK_RESPONSE );
      e_start.notify();
      offset+=sc_time(10, SC_NS);
      break;

   case TLM_READ_COMMAND://----------------RETURN RESULTS
         
      buf=(unsigned char *)&cl_num;
      pl.set_data_ptr        ( buf );
      pl.set_response_status( TLM_OK_RESPONSE );
      offset+=sc_time(5, SC_NS);
      break;

   default:
      pl.set_response_status( TLM_COMMAND_ERROR_RESPONSE );
   }

}

#endif
