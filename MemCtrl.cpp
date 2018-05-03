#ifndef MEMCTRL_C
#define MEMCTRL_C
#include "MemCtrl.hpp"

MemCtrl::MemCtrl(sc_module_name name, int& sv_num, const int sv_len,
		   sc_event *e_ready, sc_event *e_next, int& lambda,
		   int& target, deque<int>& data):sc_module(name),
						  sv_num(sv_num),
						  sv_len(sv_len),
						  e_ready(e_ready),
						  e_next(e_next),
						  lambda(lambda),
						  target(target),
						  data(data)
{
   SC_THREAD(grab_from_mem);
}
void MemCtrl::grab_from_mem()
{
   while(1)
   {
      data.clear();
      wait(*e_next);
      sv_num = num_of_sv();
      string y_line;
      ifstream y_file("../ML_number_recognition_SVM/saved_data/test_images/y.txt");
      for(int i=0; i<sv_len; i++)
      {
	 getline(y_file, y_line, ' ');
	 data.push_back(stof(y_line));
      }
      
   }
  
}
int MemCtrl::num_of_sv()
{
   int count;
   ifstream sv_file("../ML_number_recognition/saved_data/support_vectors/sv0.txt");
   if(sv_file.is_open())
   {
      while(getline(sv_file,line))
	 count++;
      sv_file.close();
   }
   else
      cout<<"error opening support vector file"<<endl;
   return count;
}
#endif
