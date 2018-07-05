#ifndef CHECKER_C
#define CHECKER_C
#include "Checker.hpp"

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
#define IMG_START SV9+(683*(SV_LEN+1))+1

const array<int, 10> sv_start_addr = {SV0, SV1, SV2, SV3, SV4, SV5, SV6, SV7, SV8, SV9};
SC_HAS_PROCESS(Checker);

Checker::Checker(sc_module_name name) : sc_module(name)
{
   cout<<name<<" constructed"<<endl;
   SC_THREAD(verify);
   match=0;
   p_port0.bind(sig0);
   p_port1.bind(sig1);
   sig0=SC_LOGIC_0;
   sig1=SC_LOGIC_0;
}


void Checker::verify()
{
   pl_t pl;
   unsigned char *buf;
   din_t *image;
   unsigned int addr = 0;
   num_t num;

   sc_time offset=SC_ZERO_TIME;
   #ifdef QUANTUM
   tlm_utils::tlm_quantumkeeper qk;
   qk.reset();
   #endif

   lines = num_of_lines("../../ML_number_recognition_SVM/saved_data/test_images/y.txt");
   labels_extraction();

   #ifdef QUANTUM
   qk.inc(sc_time(10, SC_NS));
   offset = qk.get_local_time();
   qk.set_and_sync(offset);
   #else
   offset += sc_time(10, SC_NS);
   #endif
   

   cout<<endl;
   cout<<BKG_BLUE<<BLACK<<"***** STARTING CLASSIFICATION *****"<<BKG_RST<<RST<<endl;
   cout<<BLUE<<"  -testing on "<<lines<<" images images-  \n"RST<<endl;
   
   for(int img=0; img<lines; img++)
   {
         //cout<<"READ NEW IMAGE FROM DDR"<<endl;
         //READ NEW IMAGE FROM DDR 
         addr = (IMG_START + img*SV_LEN);
         pl.set_address(addr);
         pl.set_data_length(SV_LEN);
         pl.set_command(TLM_READ_COMMAND);
         s_ch_i1->b_transport(pl, offset);
         buf = pl.get_data_ptr();
         image = ((din_t*)buf);
         assert(pl.get_response_status() == TLM_OK_RESPONSE);

         #ifdef QUANTUM
         qk.set_and_sync(offset);
         #endif
         
         //cout<<"WRITE NEW IMAGE TO BRAM"<<endl;
         //WRITE NEW IMAGE TO BRAM
         pl.set_data_ptr((unsigned char *)image);
         pl.set_address(0x80000000);
         pl.set_data_length(SV_LEN);
         pl.set_command(TLM_WRITE_COMMAND);
         s_ch_i0->b_transport(pl, offset);
         assert(pl.get_response_status() == TLM_OK_RESPONSE);

         #ifdef QUANTUM
         qk.set_and_sync(offset);
         #endif
         
         //cout<<"DUMMY FOR DSKW"<<endl;
         //DUMMY TRANSACTION TO START DSKW MODULE
         pl.set_address(0x81000000);
         pl.set_command(TLM_WRITE_COMMAND);
         s_ch_i0->b_transport(pl, offset);
         assert(pl.get_response_status() == TLM_OK_RESPONSE);
         
         #ifdef QUANTUM
         qk.inc(sc_time(10, SC_NS));
         offset = qk.get_local_time();
         qk.set_and_sync(offset);
         #endif
         
         //************** WAIT FOR DSKW TO FINISH PROCESSING IMAGE    
         //cout<<"WAITING FOR INTERRUPT"<<endl;
         do
         {
            #ifdef QUANTUM
            qk.inc(sc_time(10, SC_NS));
            offset = qk.get_local_time();
            qk.set_and_sync(offset);
            #endif
            
         tmp_sig=sig0.read();
         }
         while( tmp_sig == SC_LOGIC_0);

         //cout<<"READ DESKEWED IMAGE FROM BRAM"<<endl;
         //READ DESKEWED IMAGE FROM BRAM
         pl.set_address(0x80000000+SV_LEN);
         pl.set_data_length(SV_LEN);
         pl.set_command(TLM_READ_COMMAND);
         s_ch_i0->b_transport(pl, offset);
         buf = pl.get_data_ptr();
         assert(pl.get_response_status() == TLM_OK_RESPONSE);

         #ifdef QUANTUM
         qk.set_and_sync(offset);
         #endif
         
         //cout<<"WRITE NEW IMAGE TO DDR"<<endl;
         //WRITE NEW IMAGE TO DDR 
         addr = (IMG_START + img*SV_LEN);
         pl.set_address(addr);
         pl.set_data_ptr(buf);
         pl.set_data_length(SV_LEN);
         pl.set_command(TLM_WRITE_COMMAND);
         s_ch_i1->b_transport(pl, offset);
         assert(pl.get_response_status() == TLM_OK_RESPONSE);

         #ifdef QUANTUM
         qk.set_and_sync(offset);
         #endif

         //cout<<"START SVM"<<endl;
         //START SVM
         pl.set_address(0x82000000);
         pl.set_data_length(SV_LEN);
         pl.set_command(TLM_WRITE_COMMAND);
         s_ch_i0->b_transport(pl, offset);
         assert(pl.get_response_status() == TLM_OK_RESPONSE);
              
         #ifdef QUANTUM
         qk.inc(sc_time(10, SC_NS));
         offset = qk.get_local_time();
         qk.set_and_sync(offset);
         #endif

         //cout<<"SEND DESKEWED IMAGE TO SVM TROUGH DMA"<<endl;
         //SEND DESKEWED IMAGE - TROUGH DMA TO SVM
         pl.set_address(0x83000000 + IMG_START + img*SV_LEN);
         pl.set_data_length(SV_LEN);
         pl.set_command(TLM_WRITE_COMMAND);
         s_ch_i0->b_transport(pl, offset);
         assert(pl.get_response_status() == TLM_OK_RESPONSE);
         
         #ifdef QUANTUM
         qk.inc(sc_time(10, SC_NS));
         offset = qk.get_local_time();
         qk.set_and_sync(offset);
         #endif

         for (int core=0; core<10; core++)
         {
            for (int sv=0; sv<sv_array[core]; sv++)
            {
               do
               {
                  #ifdef QUANTUM
                  qk.inc(sc_time(10, SC_NS));
                  offset = qk.get_local_time();
                  qk.set_and_sync(offset);
                  #else
                  offset += sc_time(10, SC_NS);
                  #endif
                  
                  tmp_sig=sig1.read();
               }
               while( tmp_sig == SC_LOGIC_0);

               //if(sv==0)cout<<"SEND APPROPRIATE SV"<<endl;
               //SEND APPROPRIATE SV - TROUGH DMA TO SVM
               pl.set_address(0x83000000+sv_start_addr[core]+sv*SV_LEN);
               pl.set_data_length(SV_LEN);
               pl.set_command(TLM_WRITE_COMMAND);
               s_ch_i0->b_transport(pl, offset);
               assert(pl.get_response_status() == TLM_OK_RESPONSE);

               #ifdef QUANTUM
               qk.inc(sc_time(10, SC_NS));
               offset = qk.get_local_time();
               qk.set_and_sync(offset);
               #else
               offset += sc_time(20, SC_NS);
               #endif
               
               do
               {
                  #ifdef QUANTUM
                  qk.inc(sc_time(5, SC_NS));
                  offset = qk.get_local_time();
                  qk.set_and_sync(offset);
                  #else
                  offset += sc_time(1, SC_NS);
                  #endif
                  
                  tmp_sig=sig1.read();
               }
               while( tmp_sig == SC_LOGIC_0);

               //if(sv==0)cout<<"SEND APPROPRIATE LAMBDA"<<endl;
               //SEND APPROPRIATE LAMBDA - TROUGH DMA TO SVM
               pl.set_address(0x83000000+sv_start_addr[core]+sv_array[core]*SV_LEN+sv);
               pl.set_data_length(1);
               pl.set_command(TLM_WRITE_COMMAND);
               s_ch_i0->b_transport(pl, offset);
               assert(pl.get_response_status() == TLM_OK_RESPONSE);

               #ifdef QUANTUM
               qk.inc(sc_time(20, SC_NS));
               offset = qk.get_local_time();
               qk.set_and_sync(offset);
               #else
               offset += sc_time(20, SC_NS);
               #endif
            }
            
            do
            {
               #ifdef QUANTUM
               qk.inc(sc_time(10, SC_NS));
               offset = qk.get_local_time();
               qk.set_and_sync(offset);
               #else
               offset += sc_time(10, SC_NS);
               #endif
               
               tmp_sig=sig1.read();
            }
            while( tmp_sig == SC_LOGIC_0);
            //cout<<"SEND BIAS"<<endl;
            //SEND BIAS - TROUGH DMA TO SVM
            pl.set_address(0x83000000+sv_start_addr[core]+sv_array[core]*(SV_LEN+1));
            pl.set_data_length(1);
            pl.set_command(TLM_WRITE_COMMAND);
            s_ch_i0->b_transport(pl, offset);
            assert(pl.get_response_status() == TLM_OK_RESPONSE);

            #ifdef QUANTUM
            qk.inc(sc_time(20, SC_NS));
            offset = qk.get_local_time();
            qk.set_and_sync(offset);
            #else
            offset += sc_time(20, SC_NS);
            #endif
            
         }
         //********* WAIT FOR SVM TO FINISH CLASSIFYING THIS IMAGE 
         do
         {
            #ifdef QUANTUM
            qk.inc(sc_time(10, SC_NS));
            offset = qk.get_local_time();
            qk.set_and_sync(offset);
            #else
            offset += sc_time(10, SC_NS);
            #endif
            
            tmp_sig=sig1.read();
         }
         while( tmp_sig == SC_LOGIC_0);

         //cout<<"READ RESULTS FROM SVM"<<endl;
         //READ RESULTS FROM SVM
         pl.set_address(0x82000000);
         pl.set_data_length(1);
         pl.set_command(TLM_READ_COMMAND);
         s_ch_i0->b_transport(pl, offset);
         assert(pl.get_response_status() == TLM_OK_RESPONSE);
         num= ((num_t*)pl.get_data_ptr())[0];
         
         #ifdef QUANTUM
         qk.inc(sc_time(10, SC_NS));
         offset = qk.get_local_time();
         qk.set_and_sync(offset);
         #endif
         
         //REPORT CLASSIFICATION
         if(labels[img] == num)
         {
               cout<<B_GREEN<<"CORRECT CLASSIFICATION"<<RST<<D_GREEN<<" :: classified number: "
                  <<num<<"["<<labels[img]<<"] :true_number"<<RST;
               cout<<DIM<<"         @"<<sc_time_stamp()<<"   #"<<name()<<"  ("<<img<<")"<<RST<<endl;
            match++;
         }
         else
         {
               cout<<B_RED<<"     MISCLASSIFICATION"<<RST<<D_RED<<" :: classified number: "
                  <<num<<"["<<labels[img]<<"] :true_number"<<RST;
               cout<<DIM<<"         @"<<sc_time_stamp()<<"   #"<<name()<<"  ("<<img<<")"<<RST<<endl;
         }
   }

   //NAKON STO PRODJU SVE SLIKE 
   cout<<"Number of classifications : "<<lines<<D_MAGNETA<<"\nPercentage: "<<B_MAGNETA
    <<(float)match/lines*100<<"%\t"<<RST<<DIM<<"@"<<sc_time_stamp()<<"\t#"<<name()<<RST<<endl;

}





int Checker::num_of_lines(string str)
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

void Checker::images_extraction()
{
   int k =0;
   string  y_line;
   int lines;
   images.clear();
   ifstream y_file("../../ML_number_recognition_SVM/saved_data/test_images/y.txt");
   lines = num_of_lines("../../ML_number_recognition_SVM/saved_data/test_images/y.txt");
      if(y_file.is_open())
      {
         for(int i=0; i<SV_LEN*lines;i++)
         {
            if(k == SV_LEN-1 )
            {
               getline(y_file, y_line, '\n');
               k = 0;
            }
            else
            {
               getline(y_file, y_line, ' ');
               k++;
            }
            images.push_back(stod(y_line));
         }
      }
      else
      {
         cout<<BKG_RED<<"ERROR"<<BKG_RST<<RED<<" OPENING Y FILE"<<endl;
         cout<<RST<<DIM<<"         @"<<sc_time_stamp()<<"   #"<<name()<<RST<<endl;
      }
   y_file.close();   
}

void Checker::labels_extraction()
{
   string  l_line;
   int lines;
   labels.clear();
   ifstream l_file("../../ML_number_recognition_SVM/saved_data/labels/labels.txt");
   lines = num_of_lines("../../ML_number_recognition_SVM/saved_data/labels/labels.txt");
      if(l_file.is_open())
      {
         for(int i=0; i<lines;i++)
         {
            getline(l_file,l_line,'\n');
            labels.push_back(stoi(l_line));
         }
      }
      else
      {
         cout<<BKG_RED<<"ERROR"<<BKG_RST<<RED<<" OPENING LABEL FILE"<<endl;
         cout<<RST<<DIM<<"         @"<<sc_time_stamp()<<"   #"<<name()<<RST<<endl;
      }
   l_file.close();   
}
#endif
