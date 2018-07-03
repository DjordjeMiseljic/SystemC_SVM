#ifndef DMA_C
#define DMA_C
#include "DMA.hpp"

DMA::DMA(sc_module_name name):sc_module(name)
{
   s_dm_t.register_b_transport(this, &DMA::b_transport);
   cout<<name<<" constructed"<<endl;
   SC_THREAD(send_to_fifo);
   tmp_mem.reserve(SV_LEN);
}

void DMA::b_transport(pl_t& pl, sc_time& offset)
{
   tlm_command cmd    = pl.get_command();
   uint64 adr         = pl.get_address();
   unsigned int len   = pl.get_data_length();
   unsigned char *buf = pl.get_data_ptr();
   unsigned int start = adr-0x83000000;
    
   //cout<<"DMA : b_transport, start="<<start<<"  len="<<len<<endl;
   switch (cmd)
   {
      case TLM_WRITE_COMMAND:
         length=len;
         //TAKE REQUESTED DATA FROM DDR3 RAM
         pl.set_command(TLM_READ_COMMAND);
         pl.set_address(start);
         s_dm_i->b_transport(pl, offset);
         assert(pl.get_response_status() == TLM_OK_RESPONSE);
         //PUT IT IN TEMPORARY VARIABLE
         buf = pl.get_data_ptr();
         tmp_mem.clear();
         for(int i=0; i<len; i++)
            tmp_mem.push_back(((din_t*)buf)[i]);
         
         tmp_mem.clear();
         for(int i=0; i<len; i++)
            tmp_mem.push_back(((din_t*)buf)[i]);
         //START SENDING IT INTO FIFO
         e_send.notify();
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

void DMA::send_to_fifo()
{
   while(1)
   {
         wait(e_send);
         //PUSH IT INTO FIFO TOWARDS SVM
         for(int i=0; i<length; i++)
            p_fifo->write(tmp_mem[i]);
   }

}




#endif