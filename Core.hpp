#ifndef CORE_H
#define CORE_H
#include <systemc>
#include <deque>
#include <cstdlib>
#include <ctime>
using namespace std;
using namespace sc_core;

SC_MODULE(Core)
{
public:
	SC_HAS_PROCESS(Core);
	Core(sc_module_name name, int& sv_num, const int sv_len, sc_event *e_ready, sc_event *e_next, double& lambda, int& target, deque<double> &data);
	void proc();
protected:
	sc_event *e_next;
	sc_event *e_ready;
	double &lambda;
	int &target;
	deque <double> &data;
	deque <double> y;
	int &sv_num;
	const int sv_len;

	long double acc;
};

#endif
