#ifndef MEMCTRL_H
#define MEMCTRL_H
#include <iostream>
#include <systemc>
#include <string>
#include <fstream>
#include <vector>
#include <array>
#include <algorithm>
#include <tlm>
#include <tlm_utils/simple_target_socket.h>
#include "Format.hpp"
#include "Types.hpp"
using namespace std;
using namespace sc_core;
using namespace tlm;

class MemCtrl : public sc_module 
{
   public:
      SC_HAS_PROCESS(MemCtrl);
      MemCtrl(sc_module_name name);
      //processes
      void memory_init();
      //tlm methods
      tlm_utils::simple_target_socket<MemCtrl> s_mc_t;
      void b_transport(pl_t&, sc_time&);

      unsigned char *buffer = NULL;
   protected:
      //global variables
      
      vector <din_t> ram;
      array<int, 10> sv_lines;
      //methods
      void file_extract();
      int num_of_lines(string str);
      unsigned char* read_from_mem(uint64 address);
      int sum_of_sv(int to_element);
      void images_extraction();
};



#endif
