#ifndef MEMCTRL_C
#define MEMCTRL_C
#include "MemCtrl.hpp"

MemCtrl::MemCtrl(sc_module_name name, int& sv_num, int sv_len,
		 sc_event *e_ready, sc_event *e_next, double& lambda,
		 int& target, deque<double>& data):sc_module(name),
						   sv_num(sv_num),
						   sv_len(sv_len),
						   e_ready(e_ready),
						   e_next(e_next),
						   lambda(lambda),
						   target(target),
						   data(data)
{

   SC_THREAD(grab_from_mem);
   cout<<"Memory controller constructed"<<endl;
}
void MemCtrl::grab_from_mem()
{
   string y_line;
   string sv_line;
   string t_line;
   string l_line;
   int i=0;
   while(1)

   {
	 
      wait(*e_next);
      cout<<"first e_next received"<<endl;
      data.clear();
      sv_num = num_of_sv();
      cout<<sv_num<<endl;
      ifstream y_file("/home/nikola/Documents/git_folders/ML_number_recognition_SVM/saved_data/test_images/y.txt");
      ifstream sv_file("/home/nikola/Documents/git_folders/ML_number_recognition_SVM/saved_data/support_vectors/sv0.txt");
      ifstream l_file("/home/nikola/Documents/git_folders/ML_number_recognition_SVM/saved_data/lambdas/lambdas0.txt");
      ifstream t_file("/home/nikola/Documents/git_folders/ML_number_recognition_SVM/saved_data/targets/targets0.txt");
      if(!t_file.is_open())
	 cout<<"sv_not openede"<<endl;
      if(sv_file.is_open() && y_file.is_open() && l_file.is_open() && t_file.is_open())
      {


	 for(int i=0; i<sv_len; i++)
	 {
	    getline(y_file, y_line, ' ');
	    cout<<y_line<<endl;
	    data.push_back(stof(y_line));
	 }
	 e_ready->notify(SC_ZERO_TIME);
	 cout<<"first e_ready sent"<<endl;
	 
	 cout<<data.size()<<endl;
	 while(i<sv_num)
	 {


	    wait(*e_next);
	    cout<<"e_next received"<<endl;
	    data.clear();
	    for(int j = 0; j<sv_len; j++)
	    {
	       getline(sv_file, sv_line, ' ');
	       data.push_back(stof(sv_line));
	    }

	    getline(l_file, l_line);
	    lambda = stof(l_line);
	    
	    getline(t_file, t_line);
	    target = stof(t_line);
	    i++;
	    e_ready->notify(SC_ZERO_TIME);
	 }
      }
      else
      {
	 cout<<"couldn't open one of the files"<<endl;
	 return;
      }
      y_file.close();
      sv_file.close();
      l_file.close();
      t_file.close();
      
   }
  
}
int MemCtrl::num_of_sv()

{
   int count;
   string line;
   ifstream sv_file("/home/nikola/Documents/git_folders/ML_number_recognition_SVM/saved_data/support_vectors/sv0.txt");
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
