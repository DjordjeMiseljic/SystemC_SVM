#ifndef CLASSIFICATOR_C
#define CLASSIFICATOR_C
#include "Classificator.hpp"
Classificator::Classificator(sc_module_name name): sc_module(name),
                                                   tsoc("classificator_tsoc"),
                                                   isoc("classificator_isoc"),
                                                   dmi_valid(false)
{
   tsoc(*this);
   isoc(*this);
   
   cout<<name<<" constucted"<<endl;
}



void Classificator::b_transport(pl_t& pl, sc_time& offset)
{
   
   
   pl.set_response_status( TLM_OK_RESPONSE );
   offset += sc_time(3, SC_NS);
}


tlm_sync_enum Classificator::nb_transport_fw(pl_t& pl, phase_t& phase, sc_time& offset)
{
   return TLM_ACCEPTED;
}

bool Classificator::get_direct_mem_ptr(pl_t& pl, tlm_dmi& dmi)
{
   dmi.allow_read_write();

   return true;
}

unsigned int Classificator::transport_dbg(pl_t& pl)
{

   return 0;
}

tlm_sync_enum Classificator::nb_transport_bw(pl_t& pl, phase_t& phase, sc_time& offset)
{
   return TLM_ACCEPTED;
}

void Classificator::invalidate_direct_mem_ptr(uint64 start, uint64 end)
{
   dmi_valid = false;
}
#endif
