#ifndef CHECKER_Cp
#define CHECKER_C
#include "Checker.hpp"
#include "tlm_utils/tlm_quantumkeeper.h"

SC_HAS_PROCESS(Checker);

Checker::Checker(sc_module_name name) : sc_module(name),
                                        isoc("checker_isoc"),
                                        dmi_valid(false)
{
   cout<<name<<" constucted"<<endl;
   SC_THREAD(verify);
   isoc(*this);
}

void Checker::verify()
{
   vector<din_t> images;
   string  y_line,l_line;
   int lines;
   pl_t pl;
   sc_time offset = SC_ZERO_TIME;
   
   ifstream y_file("../../ML_number_recognition_SVM/saved_data/test_images/y.txt");
   lines = num_of_lines("../../ML_number_recognition_SVM/saved_data/test_images/y.txt");
   int k =0;

   //extracting test images
   if(y_file.is_open())
      for(int i=0; i<784*lines;i++)
         {
            if(k == 784-1 )
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
   
   // unsigned char *data =(unsigned char*)&images[0];
   // din_t *i_data = (din_t*)data;
   num_t *num;
   unsigned char* image;
   ifstream l_file("../../ML_number_recognition_SVM/saved_data/labels/labels.txt");
   if(l_file.is_open())
   {
      for(int i=0; i<10; i++)
      {
         image = (unsigned char*)&images[9*784];
         pl.set_address(1);
         pl.set_data_ptr(image);
         pl.set_command(TLM_WRITE_COMMAND);
         pl.set_data_length(784);
         isoc->b_transport(pl, offset);
         assert(pl.get_response_status() == TLM_OK_RESPONSE);
   
         pl.set_command(TLM_READ_COMMAND);
         pl.set_address(1);
         pl.set_data_length(784);
         isoc->b_transport(pl, offset);
         assert(pl.get_response_status() == TLM_OK_RESPONSE);
         num = (num_t*)pl.get_data_ptr();
         cout<<"classified number is: "<<*num;
   
         getline(l_file,l_line);
      
      
         cout<<" label is:["<<stoi(l_line)<<"]"<<endl;
      }
   }
   else
      cout<<RED<<"ERROR OPENING LABEL FILE"<<RST<<endl;
   l_file.close();


   return;
}


tlm_sync_enum Checker::nb_transport_bw(pl_t& pl, phase_t& phase, sc_time& offset)
{
	return TLM_ACCEPTED;
}

void Checker::invalidate_direct_mem_ptr(uint64 start, uint64 end)
{
	dmi_valid = false;
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
#endif
