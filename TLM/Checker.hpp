#ifndef CHECKER_H
#define CHECKER_H
#include <systemc>
#include <cstdlib>
#include <ctime>
#include <deque>
#include <string>
#include <iomanip>
#include <tlm>
#include <tlm_utils/simple_initiator_socket.h>
#include "tlm_utils/tlm_quantumkeeper.h"
#include "Format.hpp"
#include "Types.hpp"
using namespace std;
using namespace sc_core;
using namespace tlm;

class Checker : public sc_module
{
   public:
      SC_HAS_PROCESS(Checker);
      Checker(sc_module_name name);
      
      tlm_utils::simple_initiator_socket<Checker> s_ch_i0;
      tlm_utils::simple_initiator_socket<Checker> s_ch_i1;
      sc_out<sc_logic> p_port0;
      sc_out<sc_logic> p_port1;
      sc_signal <sc_logic> sig0;
      sc_signal <sc_logic> sig1;
   
   protected:
      vector<din_t> images;
      vector<num_t> labels;

      void verify();
      bool dmi_valid;
      unsigned char* dmi_mem;
      unsigned int lines;
      int match;
      sc_event e_sync;
      sc_logic tmp_sig;

      //METHODS
      void labels_extraction();
      int num_of_lines(string str);
   
};

#endif
