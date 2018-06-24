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
      
      tlm_utils::simple_initiator_socket<Checker> s_ch_i;
      sc_port<sc_signal_out_if<sc_logic>> p_port;
   
   protected:
      vector<din_t> images;
      void verify();
      bool dmi_valid;
      unsigned char* dmi_mem;
      //METHODS
      void images_extraction();
      int num_of_lines(string str);
      void monitor();   
};

#endif
