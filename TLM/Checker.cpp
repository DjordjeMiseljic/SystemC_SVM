#ifndef CHECKER_C
#define CHECKER_C
#include "Checker.hpp"
#include "tlm_utils/tlm_quantumkeeper.h"

SC_HAS_PROCESS(Checker);

Checker::Checker(sc_module_name name) : sc_module(name),
                                        isoc("checker_isoc"),
                                        dmi_valid(false)
{
   cout<<name<<" constucted"<<endl;
   SC_THREAD(verify);
   isoc(*this);
}

void Checker::verify()
{
   return;	
}


tlm_sync_enum Checker::nb_transport_bw(pl_t& pl, phase_t& phase, sc_time& offset)
{
	return TLM_ACCEPTED;
}

void Checker::invalidate_direct_mem_ptr(uint64 start, uint64 end)
{
	dmi_valid = false;
}
#endif
