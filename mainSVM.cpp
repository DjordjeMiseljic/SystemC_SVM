#include <systemc>
#include <string>
#include <deque>
#include "Core.hpp"
//#include "MemCtrl.hpp"
using namespace std;
using namespace sc_core;

int sc_main(int argc, char* argv[])
{
	Core *acc_core;
	//MemCtrl *mem_ctrl;
	int sv_num = 500;
	const int sv_len = 784;
   sc_event e_ready; // mem_ctrl->core
   sc_event e_next;	// core->mem_ctrl
	deque <double> data;
	double lambda=1;
	int target=1;

	acc_core = new Core("acc_core",sv_num, sv_len, &e_ready, &e_next, lambda, target, data);
	//mem_ctrl = new MemCtrl("mem_ctrl",sv_num, sv_len, &e_ready, &e_next, lambda, target, data);

	sc_start(200, sc_core::SC_NS);
	cout << "Simulation finished at " << sc_time_stamp() << std::endl;
	delete acc_core;
	//delete mem_ctrl;	
	return 0;
}
