#include <systemc>
#include <string>
#include <deque>
#include "Core.hpp"
#include "MemCtrl.hpp"
using namespace std;
using namespace sc_core;

int sc_main(int argc, char* argv[])
{
	Core *acc_core;
	MemCtrl *Mem_Ctrl;
	int sv_num = 500;
	const int sv_len = 784;
   sc_event e_ready; // mem_ctrl->core
   sc_event e_next;	// core->mem_ctrl
	deque <double> data;
	double lambda=1;
	int target=1;
   double res=0;

	acc_core = new Core("acc_core",sv_num, sv_len, &e_ready, &e_next, lambda, target, data, res);
	Mem_Ctrl = new MemCtrl("mem_ctrl",sv_num, sv_len, &e_ready, &e_next, lambda, target, data, res);

	sc_start(2000, sc_core::SC_NS);
	cout << "Simulation finished at " << sc_time_stamp() << std::endl;
	delete acc_core;
	delete Mem_Ctrl;	
	return 0;
}
