#ifndef DESKEW_H
#define DESKEW_H
#include <systemc>
#include <tlm>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>
#include "tlm_utils/tlm_quantumkeeper.h"
#include "Format.hpp"
#include "Types.hpp"
#include <string>
#include <array>
#include <vector>
using namespace std;
using namespace sc_core;
using namespace tlm;

class Deskew : public sc_module
{
   public:
      SC_HAS_PROCESS(Deskew);
      Deskew(sc_module_name name);
      tlm_utils::simple_initiator_socket<Deskew> s_de_i;
      tlm_utils::simple_target_socket<Deskew> s_de_t;
      sc_out<sc_logic> p_out;
   protected:
      void b_transport(pl_t&, sc_time&);
      void calc_moments(vector<din_t> image, p_t& mu11, p_t& mu02);
      void proc();
      vector<din_t> deskew(vector <din_t> image);
      vector<din_t> image;
      sc_logic toggle;
      sc_logic start;
};
#endif
