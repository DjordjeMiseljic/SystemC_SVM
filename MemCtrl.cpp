#ifndef MEMCTRL_C
#define MEMCTRL_C
#include "MemCtrl.hpp"

MemCtrl::MemCtrl(sc_module_name name, int& sv_num, int sv_len,
		 sc_event *e_ready, sc_event *e_next, double& lambda,
		 int& target, deque<double>& data, double& res):sc_module(name),
                                                                sv_num(sv_num),
                                                                sv_len(sv_len),
                                                                e_ready(e_ready),
                                                                e_next(e_next),
                                                                lambda(lambda),
                                                                target(target),
                                                                data(data),
                                                                res(res)
                                                               
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
   string r_line;
   string b_line;
   int i=0;

   ifstream y_file("../ML_number_recognition_SVM/saved_data/test_images/y.txt");
   ifstream r_file("../ML_number_recognition_SVM/saved_data/results/res.txt");
   
   while(1)
   {

      wait(*e_next);
      //cout<<"e_next received"<<endl;
      data.clear();
      sv_num = num_of_sv();
      k = 0;
      //cout<<"sv_num is: "<<sv_num<<endl;
      ifstream sv_file("../ML_number_recognition_SVM/saved_data/support_vectors/sv0.txt");
      ifstream l_file("../ML_number_recognition_SVM/saved_data/lambdas/lambdas0.txt");
      ifstream t_file("../ML_number_recognition_SVM/saved_data/targets/targets0.txt");
      ifstream b_file("../ML_number_recognition_SVM/saved_data/bias/bias.txt");

      if(sv_file.is_open() && y_file.is_open() && l_file.is_open() && t_file.is_open() && r_file.is_open() && b_file.is_open() )
      {
         for(int i=0; i<sv_len; i++)
         {
            if(i == sv_len-1 )
            {
               getline(y_file, y_line, '\n');
               data.push_back(stod(y_line));
            }
            else
            {
               getline(y_file, y_line, ' ');
               data.push_back(stod(y_line));
            }
            
         }
         cout<<"image sent"<<endl;
         getline(r_file, r_line, ' ');
         res = stod(r_line);


         getline(b_file,b_line);

         lambda = stod(b_line);
         //cout<<"bias is: "<<lambda<<endl;
         
         e_ready->notify(SC_ZERO_TIME);
         
         

         while(k<sv_num)
         {

            
            wait(*e_next);
            //cout<<"i is:"<<i<<endl;
            
            data.clear();
            for(int j = 0; j<sv_len; j++)
            {

               if(j == sv_len-1)
               {
                  getline(sv_file, sv_line, '\n');
                  data.push_back(stod(sv_line));
               }
               else
               {
                  getline(sv_file, sv_line, ' ');
                  data.push_back(stod(sv_line));
               }
               //cout<<"stod is: "<<stod(sv_line)<<"\tsv_line is: "<<sv_line<<endl;
            }
            //cout<<"data size is: "<<data.size()<<endl;
            getline(l_file, l_line);
            lambda = stod(l_line);
            //cout<<"lambda is: "<<stod(l_line)<<endl;

            getline(t_file, t_line);
            target = stod(t_line);
            //cout<<"target "<<i+1<<" is: "<<target<<endl;
            k++;

            e_ready->notify(SC_ZERO_TIME);
         }

      }
      else
      {
         cout<<"couldn't open one of the files"<<endl;
         return;
      }
      
      sv_file.close();
      l_file.close();
      t_file.close();
      b_file.close();

   }
   r_file.close();
   y_file.close();

}

int MemCtrl::num_of_sv()
{
   int count = 0;
   string line;
   ifstream sv_file("../ML_number_recognition_SVM/saved_data/support_vectors/sv0.txt");
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
