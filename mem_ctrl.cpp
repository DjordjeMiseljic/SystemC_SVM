#ifndef MEM_CTRL_C
#define MEM_CTRL_C
#include "mem_ctrl.hpp"

mem_ctrl::mem_ctrl(sc_module_name name, const int sv_num, const int sv_len,
		   sc_event *e_ready, sc_event *e_next, int *lambda,
		   int *tau, deque<int> *data):sc_module(name),
					       sv_num(sv_num),
					       sv_len(sv_len),
					       e_ready(e_ready),
					       e_next(e_next),
					       lambda(lambda),
					       tau(tau),
					       data(data)
{
  SC_THREAD(grab_from_mem);
}
					       
					       
#endif
