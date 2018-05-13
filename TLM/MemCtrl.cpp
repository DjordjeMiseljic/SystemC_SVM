#ifndef MEMCTRL_C
#define MEMCTRL_C
#include "MemCtrl.hpp"

MemCtrl::MemCtrl(sc_module_name name): sc_module(name),
                                       tsoc("mem_ctrl_tsoc"),
                                       buffer(NULL)
{
   
   tsoc(*this);
   SC_THREAD(memory_init);
   for(int i=0; i!=RAM_SIZE; i++)
      ram[i]=0;
   cout<<name<<" constructed"<<endl;
}

void MemCtrl::memory_init()
{
   file_extract();
   cout<<2*sum_of_sv(0)<<endl;
   int num =9;
   uint64 address = (2*sum_of_sv(num)-2*sv_array[num]+400)*784;
   cout<<"sum of sv is"<<sum_of_sv(9)<<endl;
   buffer = read_from_mem(address);
   // cout<<"value in lambda0 is: "<<lambdas[2][0]<<endl;
   din_t* test = (din_t*)buffer;
   //cout<<"memory lambda is:"<<*test;
   for (int i = 0; i < 784; ++i)
   {
      if(test[i]!=sv[num][399*784+i])
      {
         cout<<RED<<"ERROR MISMATCH"<<RST<<endl;
         return ;
      }
   }
    // cout<<"bias in vector: "<<biases[num]<<endl;
    // cout<<"bias in memory: "<<*test<<endl;
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
      pl.set_response_status(TLM_OK_RESPONSE);
      break;
   default:
      pl.set_response_status( TLM_COMMAND_ERROR_RESPONSE );
   }

   offset += sc_time(3, SC_NS);
}


tlm_sync_enum MemCtrl::nb_transport_fw(pl_t& pl, phase_t& phase, sc_time& offset)
{
   return TLM_ACCEPTED;
}

bool MemCtrl::get_direct_mem_ptr(pl_t& pl, tlm_dmi& dmi)
{
   dmi.allow_read_write();

   dmi.set_dmi_ptr       ( ram );
   dmi.set_start_address ( 0   );
   dmi.set_end_address   ( 199 );

   return true;
}

unsigned int MemCtrl::transport_dbg(pl_t& pl)
{
   tlm_command cmd = pl.get_command();
   unsigned char* ptr = pl.get_data_ptr();

   if ( cmd == TLM_READ_COMMAND )
      memcpy(ptr, ram, RAM_SIZE);
   else if ( cmd == TLM_WRITE_COMMAND )
      memcpy(ram, ptr, RAM_SIZE);

   return RAM_SIZE;
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
   int sv_len = 784;
   int sum = 0;
   
   for (int i = 0; i < 10; ++i)
   {
      sv[i].reserve(sv_array[i]*784);
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
      cout<<"sv "<<i<<"is "<<sv_lines[i]<<endl;
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
   cout<<"sum is: "<<sum<<endl;

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
   
   
   if(address/784<=(uint64)2*sum_of_sv(0))
      if(address/784<(uint64)sv_array[0])
         buf = (unsigned char*)&sv[0][address/784*784];
      else if(address/784 != (uint)2*sum_of_sv(0))
         buf = (unsigned char*)&lambdas[0][address/784-sum_of_sv(0)];
      else
         buf = (unsigned char*)&biases[0]; 
   
   else if(address/784<=(uint64)2*sum_of_sv(1))

      if(address/784<((uint64)2*sum_of_sv(1)-sv_array[1]))
         buf = (unsigned char*)&sv[1][(address/784-2*sum_of_sv(0)-1)*784];
      else if(address/784!=(uint)2*sum_of_sv(1))
         buf = (unsigned char*)&lambdas[1][address/784-(2*sum_of_sv(1)-sv_array[1])];
      else
         buf = (unsigned char*)&biases[1]; 
         
   else if(address/784<=(uint64)2*sum_of_sv(2))
      if(address/784<((uint64)2*sum_of_sv(2)-sv_array[2]))
         buf = (unsigned char*)&sv[2][(address/784-2*sum_of_sv(1)-1)*784];
      else if(address/784!=(uint)2*sum_of_sv(2))
         buf = (unsigned char*)&lambdas[2][address/784-(2*sum_of_sv(2)-sv_array[2])];
      else
         buf = (unsigned char*)&biases[2];
   
   else if(address/784<=(uint64)2*sum_of_sv(3))
      if(address/784<((uint64)2*sum_of_sv(3)-sv_array[3]))
         buf = (unsigned char*)&sv[3][(address/784-2*sum_of_sv(2)-1)*784];
      else if(address/784!=(uint)2*sum_of_sv(3))
         buf = (unsigned char*)&lambdas[3][address/784-(2*sum_of_sv(3)-sv_array[3])];
      else
         buf = (unsigned char*)&biases[3];
         
   else if(address/784<=(uint64)2*sum_of_sv(4))
      if(address/784<((uint64)2*sum_of_sv(4)-sv_array[4]))
         buf = (unsigned char*)&sv[4][(address/784-2*sum_of_sv(3)-1)*784];
      else if(address/784!=(uint)2*sum_of_sv(4))
         buf = (unsigned char*)&lambdas[4][address/784-(2*sum_of_sv(4)-sv_array[4])];
      else
         buf = (unsigned char*)&biases[4];
   
   else if(address/784<=(uint64)2*sum_of_sv(5))
      if(address/784<((uint64)2*sum_of_sv(5)-sv_array[5]))
         buf = (unsigned char*)&sv[5][(address/784-2*sum_of_sv(4)-1)*784];
      else if(address/784!=(uint)2*sum_of_sv(5))
         buf = (unsigned char*)&lambdas[5][address/784-(2*sum_of_sv(5)-sv_array[5])];
      else
         buf = (unsigned char*)&biases[5];

   else if(address/784<=(uint64)2*sum_of_sv(6))
      if(address/784<((uint64)2*sum_of_sv(6)-sv_array[6]))
         buf = (unsigned char*)&sv[6][(address/784-2*sum_of_sv(5)-1)*784];
      else if(address/784!=(uint)2*sum_of_sv(6))
         buf = (unsigned char*)&lambdas[6][address/784-(2*sum_of_sv(6)-sv_array[6])];   
      else
         buf = (unsigned char*)&biases[6];
   
   else if(address/784<=(uint64)2*sum_of_sv(7))
      if(address/784<((uint64)2*sum_of_sv(7)-sv_array[7]))
         buf = (unsigned char*)&sv[7][(address/784-2*sum_of_sv(6)-1)*784];
      else if(address/784!=(uint)2*sum_of_sv(7))
         buf = (unsigned char*)&lambdas[7][address/784-(2*sum_of_sv(7)-sv_array[7])];
      else
         buf = (unsigned char*)&biases[7];
         
   else if(address/784<=(uint64)2*sum_of_sv(8))
      if(address/784<((uint64)2*sum_of_sv(8)-sv_array[8]))
         buf = (unsigned char*)&sv[8][(address/784-2*sum_of_sv(7)-1)*784];
      else if(address/784!=(uint)2*sum_of_sv(8))
         buf = (unsigned char*)&lambdas[8][address/784-(2*sum_of_sv(8)-sv_array[8])];   
      else
         buf = (unsigned char*)&biases[8];
   
   else if(address/784<=(uint64)2*sum_of_sv(9))
      if(address/784<((uint64)2*sum_of_sv(9)-sv_array[9]))
         buf = (unsigned char*)&sv[9][(address/784-2*sum_of_sv(8)-1)*784];
      else if(address/784!=(uint)2*sum_of_sv(9))
         buf = (unsigned char*)&lambdas[9][address/784-(2*sum_of_sv(9)-sv_array[9])];
      else
         buf = (unsigned char*)&biases[9];
   else
      cout<<"address error"<<endl;
         

   return buf;

}
int MemCtrl::sum_of_sv(int to_element)
{
   int sum = 0;
   for(int i=0;i<=to_element;i++ )
      sum += sv_array[i];
   
   return sum;
}
#endif
