#ifndef DMI_H
#define DMI_H
#include <systemc>
#include <tlm>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>
#include "Format.hpp"
#include "Types.hpp"
#include <string>
#include <array>
#include <vector>
using namespace std;
using namespace sc_core;
using namespace tlm;

class DMI : public sc_module
{
   public:
      SC_HAS_PROCESS(DMI);
      DMI(sc_module_name name);
      tlm_utils::simple_target_socket<DMI> s_dm_t;
      tlm_utils::simple_initiator_socket<DMI> s_dm_i;
      sc_port<sc_fifo_out_if<din_t>> p_fifo;
   protected:
      void b_transport(pl_t&, sc_time&);
      void send_to_fifo();
      unsigned int length;

      vector<din_t> tmp_mem;
      sc_event e_send;
};
#endif
