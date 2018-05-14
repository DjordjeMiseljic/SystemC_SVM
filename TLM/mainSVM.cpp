#include <systemc>
#include <string>
#include <deque>
#include "Classificator.hpp"
#include "MemCtrl.hpp"
#include "Checker.hpp"
#include "Types.hpp"

#define QUANTUM

using namespace std;
using namespace sc_core;

int sc_main(int argc, char* argv[])
{
	Classificator *Cl;
	MemCtrl *Mc;
   Checker *Ch;
   
	Cl = new Classificator("Classificator");
	Mc = new MemCtrl("Memory_Controller");
   Ch = new Checker("Checker");
   
   Ch->isoc(Cl->tsoc);
   Cl->isoc(Mc->tsoc);

	#ifdef QUANTUM
	tlm_global_quantum::instance().set(sc_time(10, SC_NS));
	#endif

	sc_start(20,SC_SEC);
	cout << "Simulation finished at " << sc_time_stamp() <<endl;
   
	delete Cl;
	delete Mc;	
   delete Ch;

	return 0;
}
