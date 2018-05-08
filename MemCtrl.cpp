#ifndef MEMCTRL_C
#define MEMCTRL_C
#include "Format.hpp"
#include "MemCtrl.hpp"

MemCtrl::MemCtrl(sc_module_name name, int& sv_num, int sv_len,
		 sc_event *e_ready, sc_event *e_next, din_t& lambda,
		 din_t& target, deque<din_t>& data):sc_module(name),
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
   
   int image_num = 0;
   int sv_count=0;
   int num_of_img;
   int num = 0;
   int j,k = 0;
   deque <din_t> y_deq;
   din_t y;
   din_t d;

   file_extract();
   num_of_img=num_of_lines("../ML_number_recognition_SVM/saved_data/test_images/y.txt");
   while(image_num<num_of_img)
   {
      wait(e_next[num]);
         
      sv_num = sv_lines[num];
      data.clear();
      if(num == 0)
      {  
         y_deq.clear();
         for(int i=0; i<sv_len; i++)
         {
            y = images[j];
            if(y.overflow_flag())
               cout<<"Overflow detected on images!"<<endl; 
            y_deq.push_back(y);
            j++;
         }
      }
      data=y_deq;
      lambda = biases[num];
      if(lambda.overflow_flag())
         cout<<"Overflow detected on bias!"<<endl; 

      e_ready[num].notify(SC_ZERO_TIME);
      sv_count=0;
      k = 0;
      while(sv_count<sv_lines[num])
      {         
         wait(e_next[num]);
         data.clear();
         for(int i = 0; i<sv_len; i++)
         {
            d = sv[num][k];
            if(d.overflow_flag())
               cout<<"Overflow detected on support vectors!"<<endl; 
            data.push_back(d);
            k++;
         }
         
         lambda = lambdas[num][sv_count];
         if(lambda.overflow_flag())
            cout<<"Overflow detected on lambda!"<<endl; 
         target = targets[num][sv_count];
         if(target.overflow_flag())
            cout<<"Overflow detected on target!"<<endl; 
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
            lambdas[i].push_back(1000*stod(l_line));
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
   //double max = *std::max_element((sv[0].begin()),(sv[0].end()));
   //cout<<"max element of sv[0] is: "<<max<<endl;
   deque<double> elements_max;
   deque<double> elements_min;
   for(int i=0; i<10; i++)
   {
      double max = *std::max_element((lambdas[i].begin()),(lambdas[i].end()));
      elements_max.push_back(max);
      double min = *std::min_element((lambdas[i].begin()),(lambdas[i].end()));
      elements_min.push_back(min);
      cout<<"max element of deque "<<i<<"is: "<<max;
      cout<<"\tmin element of deque "<<i<<"is: "<<min<<endl;
   }
   double max = *std::max_element((elements_max.begin()),(elements_max.end()));
   cout<<"max element overall is: "<<max;
   double min = *std::min_element((elements_min.begin()),(elements_min.end()));
   cout<<"\tmin element overall is: "<<min<<endl;
   double min_bias = *std::min_element((biases.begin()),(biases.end()));
   double max_bias = *std::max_element((biases.begin()),(biases.end()));
   for(int i=0;i<10;i++)
      cout<<biases[i]<<endl;
   cout<<"minimal bias is: "<<min_bias<<endl;
   cout<<"maximal bias is: "<<max_bias<<endl;
   
   
   
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
