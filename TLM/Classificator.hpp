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
#include "tlm_utils/tlm_quantumkeeper.h"
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
      
      sc_out<sc_logic> p_out;
      sc_port<sc_fifo_in_if<din_t>> p_fifo;

   protected:
      void b_transport(pl_t&, sc_time&);
      void classify();

      //CLASSIFICATOR VARIABLES
      vector <din_t> image_v;
      vector <res_t> res_v;
      res_t tmp_res;
      res_t max_res;
      num_t cl_num;
      acc_t acc;
      p_t p;
      sc_logic toggle;
      sc_logic start;


};

#endif
