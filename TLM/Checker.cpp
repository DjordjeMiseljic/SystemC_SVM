#ifndef CHECKER_C
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
   lines = 1;
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
   pl.set_address(1);
   pl.set_data_ptr((unsigned char*)&images[0]);
   pl.set_command(TLM_WRITE_COMMAND);
   isoc->b_transport(pl, offset);
   pl.set_command(TLM_READ_COMMAND);
   isoc->b_transport(pl, offset);
   cout<<"classified number is: "<<(int)(*(pl.get_data_ptr()))<<endl;
   
   ifstream l_file("../../ML_number_recognition_SVM/saved_data/labels/labels.txt");
   if(l_file.is_open())
      getline(l_file,l_line);
   else
      cout<<RED<<"couldnt open label file"<<RST<<endl;
   l_file.close();
   cout<<"label is: "<<l_line<<endl;

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
#endif