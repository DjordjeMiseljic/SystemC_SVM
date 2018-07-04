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
   VP vp("VP");
   Checker ch("TB");
   
   ch.s_ch_i0.bind(vp.s_vp_t0);
   ch.s_ch_i1.bind(vp.s_vp_t1);

   vp.p_out0.bind(ch.p_port0);
   vp.p_out1.bind(ch.p_port1);
	#ifdef QUANTUM
	tlm_global_quantum::instance().set(sc_time(10, SC_NS));
	#endif

	sc_start(100,SC_SEC);
	cout << "Simulation finished at " << sc_time_stamp() <<endl;
	return 0;
}
