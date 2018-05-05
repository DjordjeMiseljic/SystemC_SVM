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
   string str;
   int image_num = 0;
   int sv_count=0;
   int num_of_img;
   int num=0;
   deque <double> y_deq;

   
   num_of_img=num_of_test_img();
   ifstream y_file("../ML_number_recognition_SVM/saved_data/test_images/y.txt");
   ifstream r_file("../ML_number_recognition_SVM/saved_data/results/res.txt");


   while(image_num<num_of_img)
   {
     // cout<<"wait_next "<<num<<"\t#"<<name()<<endl;
      wait(e_next[num]);
      //cout<<"num is: "<<num<<endl;
      //cout<<"e_next received"<<endl;
      
      sv_num = num_of_sv(num);

      //cout<<"sv_num is: "<<sv_num<<endl;
      str = "../ML_number_recognition_SVM/saved_data/support_vectors/sv";
      str = str + to_string(num);
      str = str+".txt";
      ifstream sv_file(str);
      str = "../ML_number_recognition_SVM/saved_data/lambdas/lambdas";
      str = str + to_string(num);
      str = str+".txt";
      ifstream l_file(str);
      str = "../ML_number_recognition_SVM/saved_data/targets/targets";
      str = str + to_string(num);
      str = str+".txt";
      ifstream t_file(str);
      
      str = "../ML_number_recognition_SVM/saved_data/bias/bias";
      str = str + to_string(num);
      str = str+".txt";
      ifstream b_file(str);
      

      data.clear();
      if(sv_file.is_open() && y_file.is_open() && l_file.is_open() && t_file.is_open() && r_file.is_open() && b_file.is_open() )
      {
         if(num == 0)
         {  
            y_deq.clear();
            for(int i=0; i<sv_len; i++)
            {
               if(i == sv_len-1 )
                  getline(y_file, y_line, '\n');
               else
                  getline(y_file, y_line, ' ');

               y_deq.push_back(stod(y_line));
            }
         }
         data=y_deq;
         getline(b_file,b_line);
         lambda = stod(b_line);
         //cout<<"image num "<<image_num++<<"sent"<<endl;
         if(num == 9)
            getline(r_file, r_line, '\n');
         else
            getline(r_file, r_line, ' ');

         res = stod(r_line);
         

         
         //cout<<"rdy notify "<<num<<"------->\t#"<<name()<<endl;
         e_ready[num].notify(SC_ZERO_TIME);
         sv_count=0;
         while(sv_count<sv_num)
         {
            //cout<<"wait_next "<<num<<"\t#"<<name()<<endl;
            wait(e_next[num]);
            //cout<<"i is:"<<i<<endl;
            //cout<<"e_next recieved"<<endl;
            data.clear();
            for(int j = 0; j<sv_len; j++)
            {

               if(j == sv_len-1)
                  getline(sv_file, sv_line, '\n');
               else
                  getline(sv_file, sv_line, ' ');

               data.push_back(stod(sv_line));  
               //cout<<"stod is: "<<stod(sv_line)<<"\tsv_line is: "<<sv_line<<endl;
            }
            //cout<<"data size is: "<<data.size()<<endl;
            getline(l_file, l_line);
            lambda = stod(l_line);
            //cout<<"lambda is: "<<stod(l_line)<<endl;

            getline(t_file, t_line);
            target = stod(t_line);
            //cout<<"target "<<i+1<<" is: "<<target<<endl;
            sv_count++;

            //cout<<"rdy notify "<<num<<"------->\t#"<<name()<<endl;
            e_ready[num].notify(SC_ZERO_TIME);
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
      if(num<9)
         num++;
      else
      {
         image_num++;
         num=0;
      }  
   }
   r_file.close();
   y_file.close();

}

int MemCtrl::num_of_sv(int num)
{
   int count = 0;
   string line,str;
   str = "../ML_number_recognition_SVM/saved_data/support_vectors/sv";
   str = str + to_string(num);
   str = str+".txt";
   ifstream sv_file(str);
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
int MemCtrl::num_of_test_img()
{
   int count = 0;
   string line;
   ifstream y_file("../ML_number_recognition_SVM/saved_data/test_images/y.txt");
   if(y_file.is_open())

   {
      while(getline(y_file,line))
         count++;
      y_file.close();
   }
   else
      cout<<"error opening support vector file"<<endl;
   return count;
}

#endif
