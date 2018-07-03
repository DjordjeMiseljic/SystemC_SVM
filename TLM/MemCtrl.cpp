#ifndef MEMCTRL_C
#define MEMCTRL_C
#include "MemCtrl.hpp"

MemCtrl::MemCtrl(sc_module_name name): sc_module(name),
                                       buffer(NULL)
{
   
   s_mc_t.register_b_transport(this, &MemCtrl::b_transport);
   SC_THREAD(memory_init);
   
   // for(int i=0; i!=RAM_SIZE; i++)
   //    ram[i]=0;
   cout<<name<<" constructed"<<endl;

}

void MemCtrl::memory_init()
{
   vector<din_t> test_image;
   test_image.reserve(784);
   uint64 addr;

   file_extract();
   // cout<<"sum of sv: "<<sum_of_sv(9)*784<<endl;
   // cout<<"sum of lambdas: "<<sum_of_sv(9)<<endl;
   // addr = sum_of_sv(9)*SV_LEN+sum_of_sv(9)+10;
   // unsigned char  *test =read_from_mem(addr);
   
   // //din_t *test2 = (lin_t*)test;
   // for (int i = 0; i < 784; ++i)
   // {
   //    cout<<"entered"<<endl;
   //    test_image.push_back(((bin_t*)test)[i]);
   //    cout<<i<<". pixel is: "<<test_image[i]<<endl;
   // }
   return;
}
void MemCtrl::b_transport(pl_t& pl, sc_time& offset)
{
   tlm_command cmd    = pl.get_command();
   uint64 adr         = pl.get_address();
   unsigned char *buf = pl.get_data_ptr();
   //unsigned int len   = pl.get_data_length();

   buf=buf;

   switch(cmd)
      {
      case TLM_WRITE_COMMAND:
         assert(cmd != TLM_WRITE_COMMAND);
         break;
      case TLM_READ_COMMAND:
         //buf = read_from_mem(adr);
         buf = (unsigned char*)&ram[adr];
         
         pl.set_data_ptr(buf);
         pl.set_response_status(TLM_OK_RESPONSE);
         break;
      default:
         pl.set_response_status( TLM_COMMAND_ERROR_RESPONSE );
      }

   offset += sc_time(5, SC_NS);
}


void MemCtrl::file_extract()
{
   
   string sv_line;
   string t_line;
   string l_line;
   string b_line;
   string str;
   int lines;
   int j, k=0;
   int sv_len = SV_LEN;
   int sum = 0;
   int sv_num = 5076;
   
   ram.reserve(sv_num * SV_LEN + sv_num + 10);
   
   for(int i=0; i<10; i++)
      {
         //extracting support vectors
         str = "../../ML_number_recognition_SVM/saved_data/support_vectors/sv";
         str = str + to_string(i);
         str = str+".txt";
         lines = num_of_lines(str);
         sv_lines[i] = lines;
         sum += lines;
         cout<<"number of sv for "<<i<<". clasificator is: "<<sv_lines[i]<<endl;
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
                  ram.push_back(stod(sv_line));
                  j++;
               }
         else
            cout<<RED<<"ERROR OPENING SV_FILE number: "<<i<<RST<<endl;
         sv_file.close();
         j = 0;
         k = 0;
         //extracting lambdas and targets
         lines = num_of_lines(str);
         str = "../../ML_number_recognition_SVM/saved_data/lambdas/lambdas";
         str = str + to_string(i);
         str = str+".txt";
         ifstream l_file(str);

         str = "../../ML_number_recognition_SVM/saved_data/targets/targets";
         str = str + to_string(i);
         str = str+".txt";
         ifstream t_file(str);
         if(t_file.is_open() && l_file.is_open())
            while(j != lines)
               {
                  //extracting lambda
                  getline(l_file,l_line);
                  //extracting target
                  getline(t_file,t_line);
                  //lambdas[i].push_back(stod(t_line)*1000*stod(l_line));
                  ram.push_back(stod(t_line)*1000*stod(l_line));
                  j++;
               }
         else
            cout<<RED<<"ERROR OPENING L_FILE of T_FILE number: "<<i<<RST<<endl;
         j = 0;
         //extracting biases
         str = "../../ML_number_recognition_SVM/saved_data/bias/bias";
         str = str + to_string(i);
         str = str+".txt";
         ifstream b_file(str);
         getline(b_file,b_line);
         //biases[i] = stod(b_line);
         ram.push_back(stod(b_line));
         l_file.close();
         t_file.close();
         b_file.close();
      }
   images_extraction();
   
}

int MemCtrl::num_of_lines(string str)
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

int MemCtrl::sum_of_sv(int to_element)
{
   if(to_element == -1)
      return 0;
   int sum = 0;
   for(int i=0;i<=to_element;i++ )
      sum += sv_array[i];
   
   return sum;
}


void MemCtrl::images_extraction()
{
   int k =0;
   string  y_line;
   int lines;
   
   lines = num_of_lines("../../ML_number_recognition_SVM/saved_data/test_images/y.txt");
   ifstream y_file("../../ML_number_recognition_SVM/saved_data/test_images/y.txt");
   
   
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
               ram.push_back(stod(y_line));
            }
      }
   else
      {
         cout<<BKG_RED<<"ERROR"<<BKG_RST<<RED<<" OPENING Y FILE"<<endl;
         cout<<RST<<DIM<<"         @"<<sc_time_stamp()<<"   #"<<name()<<RST<<endl;
      }
   y_file.close();   
}


#endif

         
