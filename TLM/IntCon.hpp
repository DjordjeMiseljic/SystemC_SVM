#ifndef INT_CON_H
#define INT_CON_H
#include <systemc>
#include <tlm>
#include <tlm_utils/simple_target_socket.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <iostream>
#include <string>
#include <fstream>
#include <deque>
#include <vector>
#include <array>
#include <algorithm>
#include "Format.hpp"
#include "Types.hpp"
using namespace sc_core;
class IntCon:
   public sc_core::sc_module
{
   public:
      IntCon(sc_module_name name);
      tlm_utils::simple_target_socket<IntCon> s_ic_t;
      tlm_utils::simple_initiator_socket<IntCon> s_ic_i;

   protected:
      void b_transport(pl_t&, sc_core::sc_time&);
};


#endif
