#ifndef MEMCTRL_H
#define MEMCTRL_H
#include <iostream>
#include <systemc>
#include <string>
#include <fstream>
#include <deque>
#include <vector>
#include <array>
#include <algorithm>
#include <tlm>

#include "Format.hpp"
#include "Types.hpp"
using namespace std;
using namespace sc_core;
using namespace tlm;

class MemCtrl :
   public sc_module,
   public tlm_fw_transport_if<>
{
   public:
      SC_HAS_PROCESS(MemCtrl);
      MemCtrl(sc_module_name name);
      //processes
      void memory_init();
      //tlm methods
      tlm_target_socket<> tsoc;
      void b_transport(pl_t&, sc_time&);
      tlm_sync_enum nb_transport_fw(pl_t&, phase_t&, sc_core::sc_time&);
      bool get_direct_mem_ptr(pl_t&, tlm_dmi&);
      unsigned int transport_dbg(pl_t&);

   protected:
      static const int RAM_SIZE = 1024;
      unsigned char ram[RAM_SIZE];
      //global variables
      array<deque<din_t>, 10> sv;
      array<deque<lin_t>, 10> lambdas;
      array<bin_t, 10> biases;
      array<int, 10> sv_lines;
      //methods
      void file_extract();
      int num_of_lines(string str);
};


#endif
