#ifndef CHECKER_Cp
#define CHECKER_C
#include "Checker.hpp"
#include "tlm_utils/tlm_quantumkeeper.h"

SC_HAS_PROCESS(Checker);

Checker::Checker(sc_module_name name) : sc_module(name)
{
   cout<<name<<" constucted"<<endl;
   SC_THREAD(verify);
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
   if(l_file.is_open())
   {
      for(int i=0; i<lines; i++)
      {

         #ifdef QUANTUM
         qk.inc(sc_time(4, SC_NS));
         offset = qk.get_local_time();
         #else
         offset += sc_time(4, SC_NS);
         #endif
               
         image = (unsigned char*)&images[i*SV_LEN];
         pl.set_data_ptr(image);
         pl.set_address(0x80000000);
         pl.set_data_length(SV_LEN);
         pl.set_command(TLM_WRITE_COMMAND);
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

         pl.set_command(TLM_READ_COMMAND);
         pl.set_address(0x80000000);
         pl.set_data_length(SV_LEN);
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
   }
   else
   {
      cout<<BKG_RED<<"ERROR"<<BKG_RST<<RED<<" OPENING LABEL FILE"<<endl;
      cout<<RST<<DIM<<"         @"<<sc_time_stamp()<<"   #"<<name()<<RST<<endl;
   }
      
   l_file.close();

   return;
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
   ifstream y_file("../../ML_number_recognition_SVM/saved_data/test_images/y.txt");
   lines = num_of_lines("../../ML_number_recognition_SVM/saved_data/test_images/y.txt");
      if(y_file.is_open())
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
      else
         cout<<RED<<"ERROR OPENING Y_FILE"<<RST<<endl;
   y_file.close();   
}
#endif
