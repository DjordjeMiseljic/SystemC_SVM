#ifndef MEM_CTRL_C
#define MEM_CTRL_C
#include "mem_ctrl.hpp"

mem_ctrl::mem_ctrl(sc_module_name name, int& sv_num, const int sv_len,
		   sc_event *e_ready, sc_event *e_next, int& lambda,
		   int& target, deque<int>& data):sc_module(name),
					       sv_num(sv_num),
					       sv_len(sv_len),
					       e_ready(e_ready),
					       e_next(e_next),
					       lambda(lambda),
					       target(target),
					       data(data)
{
  SC_THREAD(grab_from_mem);
}
void mem_ctrl::grab_from_mem()
{
  while(1)
  {
    
  }
  
}
					       
#endif
