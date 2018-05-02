#include "Core.hpp"
#include <cstdlib>
#include <ctime>

Core::Core(sc_module_name name, const int sv_num, const int sv_len, sc_event *e_ready, sc_event *e_next, int& lambda, int& tau, deque<double> &data):
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
	acc=0;

}

void Core::proc()
{
	int k=sv_num;
	long double p;

	e_next->notify(SC_ZERO_TIME);
	wait(*e_ready);
	//copy inputs to y 
	for(int i=0;i<sv_len;i++)
		y.push_back(data[i]);

	while(k)
	{
		p=0;
		e_next->notify(SC_ZERO_TIME);
		wait(*e_ready);
		for(int i=0; i<sv_len; i++)
			p+=(y[i]*data[i]);
		p=p*p*p;
		p=(lambda)*p;
		p=(tau)*p;
		acc+=p;
		
		cout<<"p= "<<p<<"\tacc= "<<acc<<"\t@ "<<sc_time_stamp()<<"\t#"<<name()<<endl;
		k--;
	}
	cout<<"classification finished\nacc= "<<acc<<endl;
	return;	
}

