#ifndef CORE_H
#define CORE_H
#include <systemc>
#include <deque>
using namespace std;
using namespace sc_core;

SC_MODULE(Core)
{
public:
	SC_HAS_PROCESS(Core);
	Core(sc_module_name name, const int sv_num, const int sv_len, sc_event *e_ready, sc_event *e_next, int* lambda, int* tau, deque<int> *data);
	void proc();
protected:
	sc_event *e_next;
	sc_event *e_ready;
	int *lambda;
	int *tau;
	deque <int> *data;
	const int sv_num;
	const int sv_len;

};

#endif
