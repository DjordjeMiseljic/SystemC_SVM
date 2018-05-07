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
         sc_event *e_ready, sc_event *e_next, sc_event *e_fin, double& lambda,
         int& target, deque<double> &data, double &res, int &number, double &max_acc);
      void classify();
	protected:
		int &sv_num;
		int sv_len;
		sc_event *e_ready;
		sc_event *e_next;
      sc_event *e_fin;
		double &lambda;
		int &target;
		deque <double> &data;
      double &res;
      int &number;
		double &max_acc;

		deque <double> y;
		double acc;
      Core *cores[10];
};

#endif
