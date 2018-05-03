#ifndef MEMCTRL_H
#define MEMCTRL_H
#include <iostream>
#include <systemc>
#include <string>
#include <fstream>
#include <deque>
using namespace std;
using namespace sc_core;

SC_MODULE(MemCtrl)
{
	public:
		SC_HAS_PROCESS(MemCtrl);
		MemCtrl(sc_module_name, int&, const int,
			  sc_event*, sc_event*, int&, int&, deque<double>&);
		int num_of_sv();
	protected:
		int &sv_num;
	  	int &sv_len; 
		int &lambda;
	   int &target;
		sc_event  *e_ready
		sc_event	 *e_next;
		deque <double> &data;
		//METHODS
		void grab_from_mem();

  
};

#endif
