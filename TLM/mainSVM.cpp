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
{/*
	Classificator *Cl;
	MemCtrl *Mc;
   Checker *Ch;
   */
	Classificator Cl("Classificator");
	MemCtrl Mc("Memory_Controller");
   Checker Ch("Checker");
   
   Ch.s_ch_i.bind(Cl.s_cl_t);
   Cl.s_cl_i.bind(Mc.s_mc_t);

	#ifdef QUANTUM
	tlm_global_quantum::instance().set(sc_time(10, SC_NS));
	#endif

	sc_start(20,SC_SEC);
	cout << "Simulation finished at " << sc_time_stamp() <<endl;
   /*
	delete Cl;
	delete Mc;	
   delete Ch;
*/
	return 0;
}
