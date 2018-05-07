#ifndef MEMCTRL_C
#define MEMCTRL_C
#include "Format.hpp"
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
   int num = 0;
   int j,k = 0;
   deque <double> y_deq;

   file_extract();
   num_of_img=num_of_lines("../ML_number_recognition_SVM/saved_data/test_images/y.txt");
   //cout<<"num_of_img: "<<num_of_img<<endl;
   

   while(image_num<num_of_img)
   {
      
      //cout<<"num is:" <<num<<endl;
      wait(e_next[num]);
      sv_num = sv_lines[num];
      //cout<<"e_next received"<<endl;
      data.clear();
      if(num == 0)
      {  
         y_deq.clear();
         for(int i=0; i<sv_len; i++)
         {
            y_deq.push_back(images[j]);
            //cout<<"images["<<j<<"]: "<<images[j]<<endl;
            j++;
         }
      }
      data=y_deq;
      
      lambda = biases[num];

         
      e_ready[num].notify(SC_ZERO_TIME);
      sv_count=0;
      k = 0;
      //cout<<"sv_lines num: "<<sv_lines[num]<<endl;
      while(sv_count<sv_lines[num])
      {
         
         //cout<<"waiting_for e_next2"<<endl;
         wait(e_next[num]);
         data.clear();
         for(int i = 0; i<sv_len; i++)
         {
            data.push_back(sv[num][k]);
            //cout<<"sv["<<num<<"]"<<"["<<k<<"]"<<"is: "<<sv[num][k]<<endl;
            k++;
         }
         
         lambda = lambdas[num][sv_count];
         
         target = targets[num][sv_count];
         sv_count++;
         e_ready[num].notify(SC_ZERO_TIME);
      }
      
      if(num<9)
         num++;
      else
      {
         image_num++;
         num=0;
      }  
   }
   

}




void MemCtrl::file_extract()
{
   string y_line;
   string sv_line;
   string t_line;
   string l_line;
   string b_line;
   string str;
   int lines;
   int j, k=0;
   
   lines=num_of_lines("../ML_number_recognition_SVM/saved_data/test_images/y.txt");
   //cout<<"num of images: "<<lines<<endl;
   ifstream y_file("../ML_number_recognition_SVM/saved_data/test_images/y.txt");

   
   //extracting test images
   if(y_file.is_open())
      for(int i=0; i<sv_len*lines;i++)
      {
         if(k == sv_len-1 )
         {
            getline(y_file, y_line, '\n');
            k = 0;
         }
         else
         {
            getline(y_file, y_line, ' ');
            k++;
         }
         //cout<<"image pixel "<<i<<" is: "<<stod(y_line)<<endl;
         images.push_back(stod(y_line));
      }
   else
      cout<<RED<<"ERROR OPENING Y_FILE"<<RST<<endl;
   
   y_file.close();
   j = 0;
   k = 0;
   for(int i=0; i<10; i++)
   {
      
      //extracting biases
      str = "../ML_number_recognition_SVM/saved_data/bias/bias";
      str = str + to_string(i);
      str = str+".txt";
      ifstream b_file(str);
      getline(b_file,b_line);
      biases[i] = stod(b_line);
      //cout<<"biases: "<<biases[i]<<endl;
      //extracting support vectors
      str = "../ML_number_recognition_SVM/saved_data/support_vectors/sv";
      str = str + to_string(i);
      str = str+".txt";
      lines = num_of_lines(str);
      sv_lines[i] = lines;
      ifstream sv_file(str);
      if(sv_file.is_open())
         while(j!=lines*sv_len)
         {

            if(k == sv_len-1)
            {
               getline(sv_file, sv_line, '\n');
               k = 0;
            }
            else
            {
               getline(sv_file, sv_line, ' ');
               k++;
            }
            sv[i].push_back(stod(sv_line));
            j++;
         }
      else
         cout<<RED<<"ERROR OPENING SV_FILE number: "<<i<<RST<<endl;
      sv_file.close();
      j = 0;
      k = 0;
      //extracting lambdas and targets
      lines = num_of_lines(str);
      str = "../ML_number_recognition_SVM/saved_data/lambdas/lambdas";
      str = str + to_string(i);
      str = str+".txt";
      ifstream l_file(str);

      str = "../ML_number_recognition_SVM/saved_data/targets/targets";
      str = str + to_string(i);
      str = str+".txt";
      ifstream t_file(str);
      if(t_file.is_open() && l_file.is_open())
         while(j != lines)
         {
            //extracting lambda
            getline(l_file,l_line);
            lambdas[i].push_back(stod(l_line));
            //extracting target
            getline(t_file,t_line);
            targets[i].push_back(stod(t_line));
            j++;
         }
      else
         cout<<RED<<"ERROR OPENING L_FILE of T_FILE number: "<<i<<RST<<endl;
      j = 0;
      l_file.close();
      t_file.close();
      b_file.close();
   }

}

int MemCtrl::num_of_lines(string str)
{
   int count = 0;
   string line;
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
#endif
