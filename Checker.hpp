#ifndef CHECKER_H
#define CHECKER_H
#include <systemc>
#include <deque>
#include <cstdlib>
#include <ctime>
#include <string>
using namespace std;
using namespace sc_core;

SC_MODULE(Checker)
{
   public:
      SC_HAS_PROCESS(Checker);
      Checker(sc_module_name name, sc_event *e_fin, double &res, int &number, double &max_acc);
      void verify();
      int num_of_lines(string str);
	protected:
      sc_event *e_fin;
      double &res;
      int &number;
      double &max_acc; 
   };

#endif
