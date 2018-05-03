#ifndef MEMCTRL_C
#define MEMCTRL_C
#include "MemCtrl.hpp"

MemCtrl::MemCtrl(sc_module_name name, int& sv_num, const int sv_len,
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
	 data.clear();
	 wait(*e_next);
	 sv_num = num_of_sv();
	 string y_line;
	 ifstream y_file("/home/nikola/Documents/git_folders/ML_number_recognition_SVM/saved_data/test_images/y.txt");
	 ifstream sv_file("/home/nikola/Documents/git_folders/ML_number_recognition_SVM/saved_data/support_vectors/sv0.txt");
	 ifstream l_file("/home/nikola/Documents/git_folders/ML_number_recognition_SVM/saved_data/lambdas/lambdas0.txt");
	 ifstream t_file("/home/nikola/Documents/git_folders/ML_number_recognition_SVM/saved_data/targets/target0.txt");

	 
	 for(int i=0; i<sv_len; i++)
	    {
	       if(getline(y_file, y_line, ' '))
		  data.push_back(stof(y_line));
	       else
		  cout<<"couldn't open y_file"<<endl;
	    }
	 e_ready->notify(SC_ZERO_TIME);
	 while(i<sv_num)
	    {

	       wait(*e_next);
	       data.clear();
	       for(int j = 0; j<sv_len; j++)
		  {
		     if(getline(sv_file, sv_line, ' '))
			data.push_back(stof(sv_line));
		     else
			cout<<"couldnt open sv_file"<<endl;
		  }
	       if(getline(l_file, l_line))
		  lambda = stof(l_line);
	       else
		  cout<<"couldnt open lambda_file"<<endl;
	       if(getline(t_file, t_line))
		  target = stof(t_line);
	       else
		  cout<<"couldnt open target_file"<<endl;
	       e_ready->notify(SC_ZERO_TIME);
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
