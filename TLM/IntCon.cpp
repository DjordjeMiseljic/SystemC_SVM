#ifndef INT_CON_C
#define INT_CON_C
#include "IntCon.hpp"

IntCon::IntCon(sc_module_name name):sc_module(name)
{
   s_ic_t.register_b_transport(this, &IntCon::b_transport);
}

void IntCon::b_transport(pl_t& pl, sc_time& offset)
{
   uint64 address = pl.get_address();
   string msg;
   stringstream ss;
   offset += sc_time(2, SC_NS);

   switch(address & 0xFF000000)
   {
      case 0x80000000:
         s_ic_i0->b_transport(pl, offset);
         break; 

      case 0x81000000:
         s_ic_i1->b_transport(pl, offset);
         break; 

      case 0x82000000:
         s_ic_i2->b_transport(pl, offset);
         break; 
         
      case 0x83000000:
         s_ic_i3->b_transport(pl, offset);
         break; 

      default: 
         ss<<hex<<address;
         string cmd = pl.get_command() == TLM_READ_COMMAND ? "read":"write";
         msg = "ERROR WRONG ADDRESS";
         msg += (" address: 0x" + ss.str() + " command: " + cmd + " ");
         msg +="@ "+sc_time_stamp().to_string();
         SC_REPORT_INFO("IntCon ", msg.c_str());
         break;
   }

}

#endif
