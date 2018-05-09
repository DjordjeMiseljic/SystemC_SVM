#ifndef CLASSIFICATOR_H
#define CLASSIFICATOR_H
#include <systemc>
#include <cstdlib>
#include <ctime>
#include "Core.hpp"
#include "Types.hpp"

using namespace std;
using namespace sc_core;

SC_MODULE(Classificator)
{
   public:
      SC_HAS_PROCESS(Classificator);
      Classificator(sc_module_name name, int& sv_num, int sv_len,
         sc_event *e_ready, sc_event *e_next, sc_event *e_fin, lin_t& lambda,
         din_t &target, deque<din_t> &data, res_t &res, num_t &number);
      void classify();
	protected:
		int &sv_num;
		int sv_len;
		sc_event *e_ready;
		sc_event *e_next;
      sc_event *e_fin;
		lin_t &lambda;
		din_t &target;
		deque <din_t> &data;
      res_t &res;
      num_t &number;

		deque <din_t> y;
		acc_t acc;
      Core *cores[10];
};

#endif
