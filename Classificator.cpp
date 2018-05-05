#include "Classificator.hpp"

Classificator::Classificator(sc_module_name name, int& sv_num, int sv_len, 
           sc_event *e_ready, sc_event *e_next, double& lambda,
           int& target, deque<double> &data, double &res):sv_num(sv_num),
                                                          sv_len(sv_len),
                                                          e_ready(e_ready),
                                                          e_next(e_next),
                                                          lambda(lambda),
                                                          target(target),
                                                          data(data),
                                                          res(res)
{
   cout<<"Classificator constucted"<<endl;
   SC_THREAD(proc);
   acc=0;
}

void Classificator::proc()
{
   return;	
}

