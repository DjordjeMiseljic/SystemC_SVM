#ifndef CLASSIFICATOR_H
#define CLASSIFICATOR_H
#include <systemc>
#include <cstdlib>
#include <ctime>
#include <tlm>
#include "Types.hpp"

using namespace std;
using namespace sc_core;
using namespace tlm;

class Classificator :
   public sc_module,
	public tlm_fw_transport_if<>,
   public tlm_bw_transport_if<>
{
   public:
      Classificator(sc_module_name name);

      tlm_target_socket<> tsoc;
      void b_transport(pl_t&, sc_time&);
      tlm_sync_enum nb_transport_fw(pl_t&, phase_t&, sc_core::sc_time&);
      bool get_direct_mem_ptr(pl_t&, tlm_dmi&);
      unsigned int transport_dbg(pl_t&);

      tlm_initiator_socket<> isoc;
      tlm_sync_enum nb_transport_bw(pl_t&, phase_t&, sc_time&);
      void invalidate_direct_mem_ptr(uint64, uint64);

	protected:
      bool dmi_valid;
      unsigned char* dmi_mem; 
};

#endif
