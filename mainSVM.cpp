#include <systemc>
#include <string>
#include <deque>
#include "Classificator.hpp"
#include "MemCtrl.hpp"
using namespace std;
using namespace sc_core;

int sc_main(int argc, char* argv[])
{
	Classificator *Cl;
	MemCtrl *Mem_Ctrl;
	int sv_num = 500;
	const int sv_len = 784;
   sc_event e_ready[10];
   sc_event e_next[10];
	deque <double> data;
	double lambda=1;
	int target=1;
   double res=0;
   
	Cl = new Classificator("acc_core",sv_num, sv_len, e_ready, e_next, lambda, target, data, res);
	Mem_Ctrl = new MemCtrl("mem_ctrl",sv_num, sv_len, e_ready, e_next, lambda, target, data, res);

	sc_start(10, sc_core::SC_US);
	cout << "Simulation finished at " << sc_time_stamp() << std::endl;
	delete Cl;
	delete Mem_Ctrl;	
	return 0;
}
