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
      Core(sc_module_name name, int& sv_num, int sv_len,
         sc_event *e_ready, sc_event *e_next, sc_event *e_fin, double& lambda,
         int& target, deque<double> &data, double &res, double &max_acc);
      void proc();
      double get_acc();
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
      double &max_acc;

      deque <double> y;
      double acc;
      static double maxAcc;
   };

#endif
