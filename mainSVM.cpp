#include <systemc>
#include <string>
#include <deque>
#include "Classificator.hpp"
#include "MemCtrl.hpp"
#include "Checker.hpp"
#include "Types.hpp"


using namespace std;
using namespace sc_core;

int sc_main(int argc, char* argv[])
{
	Classificator *Cl;
	MemCtrl *Mem_Ctrl;
   Checker *Check;
   
   num_t number; 
	int sv_num = 500;
	const int sv_len = 784;
   sc_event e_ready[10];
   sc_event e_next[10];
   sc_event e_fin;
	deque <din_t> data;
	lin_t lambda;
	din_t target;
   res_t res;
   
	Cl = new Classificator("classificator",sv_num, sv_len, e_ready, e_next, &e_fin, lambda, target, data, res, number);
	Mem_Ctrl = new MemCtrl("mem_ctrl",sv_num, sv_len, e_ready, e_next, lambda, target, data);
   Check = new Checker("checker", &e_fin, res, number);

	sc_start(20, sc_core::SC_SEC);
	cout << "Simulation finished at " << sc_time_stamp() << std::endl;
   
	delete Cl;
	delete Mem_Ctrl;	
   delete Check;

	return 0;
}
