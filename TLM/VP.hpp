#ifndef _VP_HPP_
#define _VP_HPP_

#include <systemc>
#include <tlm_utils/simple_target_socket.h>
#include <tlm_utils/simple_initiator_socket.h>
#include "IntCon.hpp"
#include "Classificator.hpp"
#include "MemCtrl.hpp"
#include "Types.hpp"

using namespace sc_core;
using namespace std;

class VP : sc_module
{
   public:
      VP(sc_module_name);
      tlm_utils::simple_target_socket<VP> s_vp_t;

   protected:
      tlm_utils::simple_initiator_socket<VP> s_vp_i;
      IntCon ic;
      Classificator cl;
      MemCtrl mc;

      typedef tlm::tlm_base_protocol_types::tlm_payload_type pl_t;
      void b_transport(pl_t&, sc_core::sc_time&);
};

#endif