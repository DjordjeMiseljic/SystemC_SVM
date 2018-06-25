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
      
      tlm_utils::simple_initiator_socket<Checker> s_ch_i;
      sc_port<sc_signal_out_if<sc_logic>> p_port0;
      sc_port<sc_signal_out_if<sc_logic>> p_port1;
      
      tlm_utils::tlm_quantumkeeper qk;
      sc_time offset;
   
   protected:
      vector<din_t> images;
      vector<num_t> labels;

      void verify();
      bool dmi_valid;
      unsigned char* dmi_mem;
      unsigned int lines;
      int match;

      //VARIABLES FOR ISR
      unsigned int img; //number of test image
      unsigned int core; //number of current svm core
      unsigned int sv; //number of current support vector for the core
      unsigned int lmb; //number of current lambda for the core 
      
      //METHODS
      void images_extraction();
      void labels_extraction();
      int num_of_lines(string str);
      void deskew_isr();   
      void classificator_isr();   
   
};

#endif
