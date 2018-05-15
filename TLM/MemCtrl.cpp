#ifndef MEMCTRL_C
#define MEMCTRL_C
#include "MemCtrl.hpp"

MemCtrl::MemCtrl(sc_module_name name): sc_module(name),
                                       tsoc("mem_ctrl_tsoc"),
                                       buffer(NULL)
{
   
   tsoc.register_b_transport(this, &MemCtrl::b_transport);
   SC_THREAD(memory_init);
   for(int i=0; i!=RAM_SIZE; i++)
      ram[i]=0;
   cout<<name<<" constructed"<<endl;
}

void MemCtrl::memory_init()
{
   file_extract();
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
         buf = read_from_mem(adr);
         //cout<<*((lin_t*)buf)<<endl;
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
   
   for (int i = 0; i < 10; ++i)
      {
         sv[i].reserve(sv_array[i]*SV_LEN);
         lambdas[i].reserve(sv_array[i]);
      }

   for(int i=0; i<10; i++)
      {
      
         //extracting biases
         str = "../../ML_number_recognition_SVM/saved_data/bias/bias";
         str = str + to_string(i);
         str = str+".txt";
         ifstream b_file(str);
         getline(b_file,b_line);
         biases[i] = stod(b_line);
         //extracting support vectors
         str = "../../ML_number_recognition_SVM/saved_data/support_vectors/sv";
         str = str + to_string(i);
         str = str+".txt";
         lines = num_of_lines(str);
         sv_lines[i] = lines;
         sum += lines;
         //cout<<"sv "<<i<<"is "<<sv_lines[i]<<endl;
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
                  lambdas[i].push_back(stod(t_line)*1000*stod(l_line));
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

unsigned char* MemCtrl::read_from_mem(uint64 address)
{
   unsigned char *buf;
   //first sv,lambda and bias
   if(address <= ((uint64)sum_of_sv(0))*SV_LEN+sum_of_sv(0))
      if(address<((uint64)sum_of_sv(0))*SV_LEN)
         buf = (unsigned char*)&sv[0][address];
      else if(address!=(uint64)(sum_of_sv(0))*SV_LEN+sum_of_sv(0))
         buf = (unsigned char*)&lambdas[0][address - (sum_of_sv(0))*SV_LEN];
      else
         buf = (unsigned char*)&biases[0]; 

   else if(address <= ((uint)sum_of_sv(1))*SV_LEN+sum_of_sv(1)+1)
      if(address<=((uint64)sum_of_sv(1))*SV_LEN+sum_of_sv(0))
         buf = (unsigned char*)&sv[1][address-sum_of_sv(0)*SV_LEN-sum_of_sv(0)-1];
      else if(address!=(uint64)(sum_of_sv(1))*SV_LEN+sum_of_sv(1)+1)
         buf = (unsigned char*)&lambdas[1][address-(sum_of_sv(1)*SV_LEN+sum_of_sv(0)+1)];
      else
         buf = (unsigned char*)&biases[1]; 

   else if(address <= ((uint)sum_of_sv(2))*SV_LEN+sum_of_sv(2)+2)
      if(address<=((uint64)sum_of_sv(2))*SV_LEN+sum_of_sv(1))
         buf = (unsigned char*)&sv[2][address-sum_of_sv(1)*SV_LEN-sum_of_sv(1)-2];
      else if(address!=(uint64)(sum_of_sv(2))*SV_LEN+sum_of_sv(2)+2)
         buf = (unsigned char*)&lambdas[2][address - (sum_of_sv(2)*SV_LEN+sum_of_sv(1)+2)];
      else
         buf = (unsigned char*)&biases[2];
   
   else if(address <= ((uint)sum_of_sv(3))*SV_LEN+sum_of_sv(3)+3)
      if(address<=((uint64)sum_of_sv(3))*SV_LEN+sum_of_sv(2))
         buf = (unsigned char*)&sv[3][address-sum_of_sv(2)*SV_LEN-sum_of_sv(2)-3];
      else if(address!=(uint64)(sum_of_sv(3))*SV_LEN+sum_of_sv(3)+3)
         buf = (unsigned char*)&lambdas[3][address - (sum_of_sv(3)*SV_LEN+sum_of_sv(2)+3)];
      else
         buf = (unsigned char*)&biases[3];

   else if(address <= ((uint)sum_of_sv(4))*SV_LEN+sum_of_sv(4)+4)
      if(address<=((uint64)sum_of_sv(4))*SV_LEN+sum_of_sv(3))
         buf = (unsigned char*)&sv[4][address-sum_of_sv(3)*SV_LEN-sum_of_sv(3)-4];
      else if(address!=(uint64)(sum_of_sv(4))*SV_LEN+sum_of_sv(4)+4)
         buf = (unsigned char*)&lambdas[4][address - (sum_of_sv(4)*SV_LEN+sum_of_sv(3)+4)];
      else
         buf = (unsigned char*)&biases[4];

   else if(address <= ((uint)sum_of_sv(5))*SV_LEN+sum_of_sv(5)+5)
      if(address<=((uint64)sum_of_sv(5))*SV_LEN+sum_of_sv(4))
         buf = (unsigned char*)&sv[5][address-sum_of_sv(4)*SV_LEN-sum_of_sv(4)-5];
      else if(address!=(uint64)(sum_of_sv(5))*SV_LEN+sum_of_sv(5)+5)
         buf = (unsigned char*)&lambdas[5][address - (sum_of_sv(5)*SV_LEN+sum_of_sv(4)+5)];
      else
         buf = (unsigned char*)&biases[5];
   
   else if(address <= ((uint)sum_of_sv(6))*SV_LEN+sum_of_sv(6)+6)
      if(address<=((uint64)sum_of_sv(6))*SV_LEN+sum_of_sv(5))
         buf = (unsigned char*)&sv[6][address-sum_of_sv(5)*SV_LEN-sum_of_sv(5)-6];
      else if(address!=(uint64)(sum_of_sv(6))*SV_LEN+sum_of_sv(6)+6)
         buf = (unsigned char*)&lambdas[6][address - (sum_of_sv(6)*SV_LEN+sum_of_sv(5)+6)];
      else
         buf = (unsigned char*)&biases[6];

   else if(address <= ((uint)sum_of_sv(7))*SV_LEN+sum_of_sv(7)+7)
      if(address<=((uint64)sum_of_sv(7))*SV_LEN+sum_of_sv(6))
         buf = (unsigned char*)&sv[7][address-sum_of_sv(6)*SV_LEN-sum_of_sv(6)-7];
      else if(address!=(uint64)(sum_of_sv(7))*SV_LEN+sum_of_sv(7)+7)
         buf = (unsigned char*)&lambdas[7][address - (sum_of_sv(7)*SV_LEN+sum_of_sv(6)+7)];
      else
         buf = (unsigned char*)&biases[7];
   
   else if(address <= ((uint)sum_of_sv(8))*SV_LEN+sum_of_sv(8)+8)
      if(address<=((uint64)sum_of_sv(8))*SV_LEN+sum_of_sv(7))
         buf = (unsigned char*)&sv[8][address-sum_of_sv(7)*SV_LEN-sum_of_sv(7)-8];
      else if(address!=(uint64)(sum_of_sv(8))*SV_LEN+sum_of_sv(8)+8)
         buf = (unsigned char*)&lambdas[8][address - (sum_of_sv(8)*SV_LEN+sum_of_sv(7)+8)];
      else
         buf = (unsigned char*)&biases[8];
   
   else if(address <= ((uint)sum_of_sv(9))*SV_LEN+sum_of_sv(9)+9)
      if(address<=((uint64)sum_of_sv(9))*SV_LEN+sum_of_sv(8))
         buf = (unsigned char*)&sv[9][address-sum_of_sv(8)*SV_LEN-sum_of_sv(8)-9];
      else if(address!=(uint64)(sum_of_sv(9))*SV_LEN+sum_of_sv(9)+9)
         buf = (unsigned char*)&lambdas[9][address - (sum_of_sv(9)*SV_LEN+sum_of_sv(8)+9)];
      else
         buf = (unsigned char*)&biases[9];
   else
      cout<<BLUE<<"ADRESS_OUT_OF_RANGE ADDRESS IS: "<<address<<RST<<endl;
   
                                                                          

         return buf;

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
#endif

         
