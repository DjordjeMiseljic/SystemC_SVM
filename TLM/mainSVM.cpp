#include <systemc>
#include <string>
#include <deque>
#include "VP.hpp"
#include "Checker.hpp"
#include "Types.hpp"


using namespace std;
using namespace sc_core;

int sc_main(int argc, char* argv[])
{
   VP vp("Virtual_Platform");
   Checker ch("Checker_TB");
   
   ch.s_ch_i.bind(vp.s_vp_t);
   ch.p_port0.bind(vp.p_exp0);
   ch.p_port1.bind(vp.p_exp1);

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
