#ifndef CORE_H
#define CORE_H
#include <systemc>
#include <deque>
#include <cstdlib>
#include <ctime>
#include "Types.hpp"

using namespace std;
using namespace sc_core;

SC_MODULE(Core)
{
   public:
      SC_HAS_PROCESS(Core);
      Core(sc_module_name name, int& sv_num, int sv_len,
         sc_event *e_ready, sc_event *e_next, sc_event *e_fin, din_t& lambda,
         din_t& target, deque<din_t> &data, din_t &res, double &max_acc);
      void proc();
      acc_t get_acc();
	protected:
		int &sv_num;
		int sv_len;
		sc_event *e_ready;
		sc_event *e_next;
      sc_event *e_fin;
      din_t &lambda;
      din_t &target;
      deque <din_t> &data;
      din_t &res;
      double &max_acc;

      deque <din_t> y;
      acc_t acc;
      static double maxAcc;
   };

#endif
