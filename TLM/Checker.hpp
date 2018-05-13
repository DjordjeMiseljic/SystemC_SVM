#ifndef CHECKER_H
#define CHECKER_H
#include <systemc>
#include <cstdlib>
#include <ctime>
#include <deque>
#include <string>
#include <iomanip>
#include <tlm>

#include "Format.hpp"
#include "Types.hpp"
using namespace std;
using namespace sc_core;
using namespace tlm;

class Checker :
   public sc_module,
   public tlm_bw_transport_if<>
{
   public:
      SC_HAS_PROCESS(Checker);
      Checker(sc_module_name name);
      
      tlm_initiator_socket<> isoc;
      tlm_sync_enum nb_transport_bw(pl_t&, phase_t&, sc_time&);
      void invalidate_direct_mem_ptr(uint64, uint64);
   
   protected:
      void verify();
      bool dmi_valid;
      unsigned char* dmi_mem;
      int num_of_lines(string str);

};

#endif
