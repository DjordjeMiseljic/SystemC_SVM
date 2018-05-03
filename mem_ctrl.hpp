#ifndef MEM_CTRL_H
#define MEM_CTRL_H
#include <iostream>
#include <systemc>
#include <string>
#include <fstream>
#include <deque>
using namespace std;
using namespace sc_core;

SC_MODULE(mem_ctrl)
{
 public:
  SC_HAS_PROCESS(mem_ctrl);
  mem_ctrl(sc_module_name, const int, const int,
	   sc_event*, sc_event*, int*, int*, deque<int>*);
 protected:
  int sv_num, sv_len, *lambda, *tau;
  sc_event  *e_ready, *e_next;
  deque <int> *data;
  //METHODS
  void grab_from_mem();
  
};

#endif
