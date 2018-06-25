#ifndef DMI_C
#define DMI_C
#include "DMI.hpp"

DMI::DMI(sc_module_name name):sc_module(name)
{
   s_dm_t.register_b_transport(this, &DMI::b_transport);
   cout<<name<<" constructed"<<endl;
   
   tmp_mem.reserve(SV_LEN);
}

void DMI::b_transport(pl_t& pl, sc_time& offset)
{
   tlm_command cmd    = pl.get_command();
   uint64 adr         = pl.get_address();
   unsigned int len   = pl.get_data_length();
   unsigned char *buf = pl.get_data_ptr();
   unsigned int start = adr-0x83000000;
    
   switch (cmd)
   {
      case TLM_WRITE_COMMAND:
         //TAKE REQUESTED DATA FROM DDR3 RAM
         pl.set_command(TLM_READ_COMMAND);
         pl.set_address((uint64)start);
         s_dm_t->b_transport(pl, offset);
         assert(pl.get_response_status() == TLM_OK_RESPONSE);
         //PUT IT IN TEMPORARY VARIABLE
         tmp_mem.clear();
         for(int i=0; i<len; i++)
            tmp_mem[i]=(((din_t*)buf)[i]);
         //PUSH IT INTO FIFO TOWARDS SVM
         for(int i=0; i<len; i++)
           fifo.write(tmp_mem[i]);
         //AFTER ITS DONE, FINISH TRANSACTION
         pl.set_response_status( TLM_OK_RESPONSE );
         offset += sc_time(10, SC_NS);
         break;

      case TLM_READ_COMMAND:
         //IF ZYNC REQUESTS DATA GIVE HIM TMP ( last data taken from ddr3)
         pl.set_data_ptr((unsigned char*)&tmp_mem[start]);
         pl.set_response_status( TLM_OK_RESPONSE );
         offset += sc_time(5, SC_NS);
         break;

      default:
         pl.set_response_status( TLM_COMMAND_ERROR_RESPONSE );
   }
}


#endif
