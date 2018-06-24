#ifndef BRAM_C
#define BRAM_C
#include "BRAM.hpp"

BRAM::BRAM(sc_module_name name):sc_module(name)
{
   s_br_t0.register_b_transport(this, &BRAM::b_transport);
   s_br_t1.register_b_transport(this, &BRAM::b_transport);
   cout<<name<<" constructed"<<endl;
   
   mem.reserve(2*SV_LEN);
}

void BRAM::b_transport(pl_t& pl, sc_time& offset)
{
   tlm_command cmd    = pl.get_command();
   uint64 adr         = pl.get_address();
   unsigned int len   = pl.get_data_length();
   unsigned char *buf = pl.get_data_ptr();
   unsigned int start = adr-0x80000000;
   if(start+len>2*SV_LEN)
   {
      cout<<RED<<"OUT OF BRAM ADRESS LIMITS"<<RST<<endl;
      pl.set_response_status( TLM_COMMAND_ERROR_RESPONSE );
      return;
   }
    
   switch (cmd)
   {
      case TLM_WRITE_COMMAND:
         for(int i=0; i<len; i++)
            mem[start+i]=(((din_t*)buf)[i]);
         pl.set_response_status( TLM_OK_RESPONSE );
         offset += sc_time(10, SC_NS);
         break;

      case TLM_READ_COMMAND:
         pl.set_data_ptr((unsigned char*)&mem[start]);
         pl.set_response_status( TLM_OK_RESPONSE );
         offset += sc_time(5, SC_NS);
         break;

      default:
         pl.set_response_status( TLM_COMMAND_ERROR_RESPONSE );
   }
}


#endif
