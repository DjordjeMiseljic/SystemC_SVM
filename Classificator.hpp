#ifndef CLASSIFICATOR_H
#define CLASSIFICATOR_H
#include <systemc>
#include <cstdlib>
#include <ctime>
#include "Core.hpp"
using namespace std;
using namespace sc_core;

SC_MODULE(Classificator)
{
   public:
      SC_HAS_PROCESS(Classificator);
      Classificator(sc_module_name name, int& sv_num, int sv_len,
         sc_event *e_ready, sc_event *e_next, double& lambda,
         int& target, deque<double> &data, double &res);
      void classify();
	protected:
		int &sv_num;
		int sv_len;
		sc_event *e_ready;
		sc_event *e_next;
		double &lambda;
		int &target;
		deque <double> &data;
      double &res;
		
		deque <double> y;
		double acc;
      sc_event e_fin;
      Core *cores[10];
};

#endif
