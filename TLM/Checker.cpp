#ifndef CHECKER_C
#define CHECKER_C
#include "Checker.hpp"
#include "tlm_utils/tlm_quantumkeeper.h"

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
SC_HAS_PROCESS(Checker);

Checker::Checker(sc_module_name name) : sc_module(name)
{
   cout<<name<<" constucted"<<endl;
   SC_THREAD(verify);

   SC_METHOD(deskew_isr);
   sensitive <<p_port0;
   dont_initialize();

   SC_METHOD(classificator_isr);
   sensitive <<p_port1;
   dont_initialize();

   img=0;
   core=0;
   sv=0;
   lmb=0;
}


void Checker::verify()
{
   pl_t pl;
   string l_line;
   int lines;
   sc_time offset = SC_ZERO_TIME;
   num_t *num;
   unsigned char* image;
   num_t label = 0;
   int match = 0;
   
   #ifdef QUANTUM
   tlm_utils::tlm_quantumkeeper qk;
   qk.reset();
   #endif
   
   lines = num_of_lines("../../ML_number_recognition_SVM/saved_data/test_images/y.txt");
   images_extraction();
   ifstream l_file("../../ML_number_recognition_SVM/saved_data/labels/labels.txt");
   labels_extraction();

      for(int i=0; i<lines; i++)
      {

         #ifdef QUANTUM
         qk.inc(sc_time(4, SC_NS));
         offset = qk.get_local_time();
         #else
         offset += sc_time(4, SC_NS);
         #endif
         
         //WRITE THE IMAGE TO BRAM
         image = (unsigned char*)&images[i*SV_LEN];
         pl.set_data_ptr(image);
         pl.set_address(0x80000000);
         pl.set_data_length(SV_LEN);
         pl.set_command(TLM_WRITE_COMMAND);
         s_ch_i->b_transport(pl, offset);
         assert(pl.get_response_status() == TLM_OK_RESPONSE);

         //DUMMY TRANSACTION TO START DSKW MODULE
         pl.set_address(0x81000000);
         pl.set_command(TLM_WRITE_COMMAND);
         s_ch_i->b_transport(pl, offset);
         assert(pl.get_response_status() == TLM_OK_RESPONSE);

         //READ DESKEWED IMAGE FROM BRAM
         pl.set_address(0x80000000+SV_LEN);
         pl.set_data_length(SV_LEN);
         pl.set_command(TLM_READ_COMMAND);
         s_ch_i->b_transport(pl, offset);
         assert(pl.get_response_status() == TLM_OK_RESPONSE);

         #ifdef QUANTUM
         qk.set_and_sync(offset);
         #endif


         #ifdef QUANTUM
         qk.inc(sc_time(4, SC_NS));
         offset = qk.get_local_time();
         #else
         offset += sc_time(4, SC_NS);
         #endif

         pl.set_address(0x82000000);
         pl.set_data_length(SV_LEN);
         pl.set_command(TLM_WRITE_COMMAND);
         s_ch_i->b_transport(pl, offset);
         assert(pl.get_response_status() == TLM_OK_RESPONSE);

         pl.set_address(0x82000000);
         pl.set_data_length(1);
         pl.set_command(TLM_READ_COMMAND);
         s_ch_i->b_transport(pl, offset);
         assert(pl.get_response_status() == TLM_OK_RESPONSE);

         num = (num_t*)pl.get_data_ptr();
   
         #ifdef QUANTUM
         qk.set_and_sync(offset);
         #endif

         getline(l_file,l_line);
         label = (num_t)stoi(l_line);
         if(label == *num)
         {
               cout<<B_GREEN<<"CORRECT CLASSIFICATION"<<RST<<D_GREEN<<" :: classified number: "
                  <<*num<<"["<<label<<"] :true_number"<<RST;
               cout<<DIM<<"         @"<<sc_time_stamp()<<"   #"<<name()<<"  ("<<i<<")"<<RST<<endl;
            match++;
         }
         else
         {
               cout<<B_RED<<"     MISCLASSIFICATION"<<RST<<D_RED<<" :: classified number: "
                  <<*num<<"["<<label<<"] :true_number"<<RST;
               cout<<DIM<<"         @"<<sc_time_stamp()<<"   #"<<name()<<"  ("<<i<<")"<<RST<<endl;
         }
         
      }
      cout<<"Number of classifications : "<<lines<<D_MAGNETA<<"\nPercentage: "<<B_MAGNETA
          <<(float)match/lines*100<<"%\t"<<RST<<DIM<<"@"<<sc_time_stamp()<<"\t#"<<name()<<RST<<endl;
      
   return;
}


void Checker::deskew_isr()
{
   cout<< "Deskew Interrupt " ;
   cout<<RST<<DIM<<"         @"<<sc_time_stamp()<<"   #"<<name()<<RST<<endl;
   

}

void Checker::classificator_isr()
{
   cout<< "SVM Interrupt " ;
   cout<<RST<<DIM<<"         @"<<sc_time_stamp()<<"   #"<<name()<<RST<<endl;

   if(sv<=img)



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
            getline(l_file,l_line);
            labels.push_back ((num_t)stoi(l_line));
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
