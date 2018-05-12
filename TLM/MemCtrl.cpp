#ifndef MEMCTRL_C
#define MEMCTRL_C
#include "MemCtrl.hpp"

MemCtrl::MemCtrl(sc_module_name name): sc_module(name),
                                       tsoc("mem_ctrl_tsoc") 
{
   tsoc(*this);
   for(int i=0; i!=RAM_SIZE; i++)
      ram[i]=0;
   cout<<name<<" controller constructed"<<endl;
}


void MemCtrl::b_transport(pl_t& pl, sc_time& offset)
{
	tlm_command cmd    = pl.get_command();
	uint64 adr         = pl.get_address();
	unsigned char *buf = pl.get_data_ptr();
	unsigned int len   = pl.get_data_length();

	switch(cmd)
	{
      case TLM_WRITE_COMMAND:
         for (unsigned int i = 0; i != len; ++i)
            ram[adr++] = buf[i];
         pl.set_response_status( TLM_OK_RESPONSE );
         break;
      case TLM_READ_COMMAND:
         for (unsigned int i = 0; i != len; ++i)
            buf[i] = ram[adr++];
         pl.set_response_status( TLM_OK_RESPONSE );
         break;
      default:
         pl.set_response_status( TLM_COMMAND_ERROR_RESPONSE );
	}

	offset += sc_time(3, SC_NS);
}


tlm_sync_enum MemCtrl::nb_transport_fw(pl_t& pl, phase_t& phase, sc_time& offset)
{
	return TLM_ACCEPTED;
}

bool MemCtrl::get_direct_mem_ptr(pl_t& pl, tlm_dmi& dmi)
{
	dmi.allow_read_write();

	dmi.set_dmi_ptr       ( ram );
	dmi.set_start_address ( 0   );
	dmi.set_end_address   ( 199 );

	return true;
}

unsigned int MemCtrl::transport_dbg(pl_t& pl)
{
	tlm_command cmd = pl.get_command();
	unsigned char* ptr = pl.get_data_ptr();

	if ( cmd == TLM_READ_COMMAND )
		memcpy(ptr, ram, RAM_SIZE);
	else if ( cmd == TLM_WRITE_COMMAND )
		memcpy(ram, ptr, RAM_SIZE);

	return RAM_SIZE;
}
#endif
