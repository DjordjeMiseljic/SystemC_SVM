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

const array<int, 10> sv_start_addr = {SV0, SV1, SV2, SV3, SV4, SV5, SV6, SV7, SV8, SV9};
SC_HAS_PROCESS(Checker);

Checker::Checker(sc_module_name name) : sc_module(name)
{
   cout<<name<<" constucted"<<endl;

   SC_METHOD(deskew_isr);
   sensitive <<p_port0;
   dont_initialize();

   SC_METHOD(classificator_isr);
   sensitive <<p_port1;
   dont_initialize();

   SC_THREAD(verify);

   offset= SC_ZERO_TIME;
   img=0;
   core=10;
   sv=0;
   lmb=0;
   match=0;
}


void Checker::verify()
{
   pl_t pl;
   unsigned char* image;

   #ifdef QUANTUM
   qk.reset();
   #endif
   
   cout<<"...file extraction"<<endl;
   lines = num_of_lines("../../ML_number_recognition_SVM/saved_data/test_images/y.txt");
   images_extraction();
   labels_extraction();

   
   #ifdef QUANTUM
   qk.set_and_sync(offset);
   #endif
}


void Checker::deskew_isr()
{
   pl_t pl;
   
   //READ DESKEWED IMAGE FROM BRAM
   pl.set_address(0x80000000+SV_LEN);
   pl.set_data_length(SV_LEN);
   pl.set_command(TLM_READ_COMMAND);
   s_ch_i->b_transport(pl, offset);
   assert(pl.get_response_status() == TLM_OK_RESPONSE);


   /*#ifdef QUANTUM
   qk.inc(sc_time(4, SC_NS));
   offset = qk.get_local_time();
   #else
   offset += sc_time(4, SC_NS);
   #endif*/

   pl.set_address(0x82000000);
   pl.set_data_length(SV_LEN);
   pl.set_command(TLM_WRITE_COMMAND);
   s_ch_i->b_transport(pl, offset);
   assert(pl.get_response_status() == TLM_OK_RESPONSE);

   /*#ifdef QUANTUM
   qk.set_and_sync(offset);
   #endif*/

   return;
}

void Checker::classificator_isr()
{
   pl_t pl;
   
   if (img==lines) //zavrsi simulaciju
   {
      //NAKON STO PRODJU SVE SLIKE 
      cout<<"Number of classifications : "<<lines<<D_MAGNETA<<"\nPercentage: "<<B_MAGNETA
       <<(float)match/lines*100<<"%\t"<<RST<<DIM<<"@"<<sc_time_stamp()<<"\t#"<<name()<<RST<<endl;
      
      /*#ifdef QUANTUM
      qk.inc(sc_time(4, SC_NS));
      offset = qk.get_local_time();
      #else
      offset += sc_time(4, SC_NS);
      #endif
   
      #ifdef QUANTUM
      qk.set_and_sync(offset);
      #endif*/

      return;
         
   }
   else if (core==10) //sledeca slika
   {
      num_t *num;
      unsigned char* image;
      
      if (img != 0)
      {

         //READ RESULTS FROM SVM
         pl.set_address(0x82000000);
         pl.set_data_length(1);
         pl.set_command(TLM_READ_COMMAND);
         s_ch_i->b_transport(pl, offset);
         assert(pl.get_response_status() == TLM_OK_RESPONSE);

         num = (num_t*)pl.get_data_ptr();
         
         //REPORT CLASSIFICATION
         if(labels[img-1] == *num)
         {
               cout<<B_GREEN<<"CORRECT CLASSIFICATION"<<RST<<D_GREEN<<" :: classified number: "
                  <<*num<<"["<<labels[img-1]<<"] :true_number"<<RST;
               cout<<DIM<<"         @"<<sc_time_stamp()<<"   #"<<name()<<"  ("<<img-1<<")"<<RST<<endl;
            match++;
         }
         else
         {
               cout<<B_RED<<"     MISCLASSIFICATION"<<RST<<D_RED<<" :: classified number: "
                  <<*num<<"["<<labels[img-1]<<"] :true_number"<<RST;
               cout<<DIM<<"         @"<<sc_time_stamp()<<"   #"<<name()<<"  ("<<img-1<<")"<<RST<<endl;
         }
         
      }

      if(img<lines)
      {
         //WRITE NEW IMAGE TO BRAM
         image = (unsigned char*)&images[img*SV_LEN];
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
      }
      
      img++;
      core=0;
      return;

   }
   else if (sv==lmb && sv!=sv_array[core]) //treba mu sledeci sv
   {
      pl.set_address(0x83000000+sv_start_addr[core]+sv*SV_LEN);
      pl.set_data_length(SV_LEN);
      pl.set_command(TLM_WRITE_COMMAND);
      s_ch_i->b_transport(pl, offset);
      assert(pl.get_response_status() == TLM_OK_RESPONSE);

      sv++;
      return;
   }
   else if(sv>lmb) //treba mu sledeca lambda
   {

      pl.set_address(0x83000000+sv_start_addr[core]+sv_array[core]*SV_LEN+sv-1);
      //-1 zato sto prethodi sv++ a treba lambda za neuvecani sv
      pl.set_data_length(1);
      pl.set_command(TLM_WRITE_COMMAND);
      s_ch_i->b_transport(pl, offset);
      assert(pl.get_response_status() == TLM_OK_RESPONSE);

      lmb++;
      return;
   }
   else if(sv==lmb && sv==sv_array[core]) //treba mu bias
   {
   
      pl.set_address(0x83000000+sv_start_addr[core]+sv_array[core]*(SV_LEN+1));
      pl.set_data_length(1);
      pl.set_command(TLM_WRITE_COMMAND);
      s_ch_i->b_transport(pl, offset);
      assert(pl.get_response_status() == TLM_OK_RESPONSE);

      sv=0;
      lmb=0;
      core++;
      return;
   }
   else
   {
      cout<<BKG_RED<<"ERROR"<<BKG_RST<<RED<<" ! FORBIDDEN STATE !"<<endl;
      cout<<RST<<DIM<<"         @"<<sc_time_stamp()<<"   #"<<name()<<RST<<endl;
      return;
   }

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
