#ifndef CLASSIFICATOR_H
#define CLASSIFICATOR_H
#include <systemc>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <array>
#include <tlm>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>
#include "Types.hpp"
#include "Format.hpp"

using namespace std;
using namespace sc_core;
using namespace tlm;

class Classificator : public sc_module
{
   public:
      SC_HAS_PROCESS(Classificator);
      Classificator(sc_module_name name);
      
      tlm_utils::simple_target_socket<Classificator> s_cl_t;
      tlm_utils::simple_initiator_socket<Classificator> s_cl_i;
      
      sc_export<sc_signal_out_if<sc_logic>> p_exp;
      sc_signal <sc_logic> s_new;
      sc_port<sc_fifo_out_if<d_in>> p_fifo;

   protected:
      void b_transport(pl_t&, sc_time&);
      //CLASSIFICATOR VARIABLES
      vector <din_t> image_v;
      vector <res_t> res_v;
      acc_t acc;
      p_t p;
      num_t cl_num;
      res_t max_res;
      sc_logic toggle;

};

#endif
