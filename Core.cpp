#include "Core.hpp"
#include <cstdlib>
#include <ctime>

Core::Core(sc_module_name name, const int sv_num, const int sv_len, sc_event *e_ready, sc_event *e_next, int* lambda, int* tau, deque<int> *data):
	sv_num(sv_num),
	sv_len(sv_len),
	e_ready(e_ready),
	e_next(e_next),
	lambda(lambda),
	tau(tau),
	data(data)
{
	cout<<"Core constucted"<<endl;
	SC_THREAD(proc);
}

void Core::proc()
{
	int i=0;
	while(i<10)
	{
		wait(10,SC_NS);
		cout<<"doot"<<"@ "<<sc_time_stamp()<<" #"<<name()<<endl;
		i++;
	}
	cout<<"ending"<<endl;
	return;	
}

