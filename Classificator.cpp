#include "Classificator.hpp"
Classificator::Classificator(sc_module_name name, int& sv_num, int sv_len, 
           sc_event *e_ready, sc_event *e_next, sc_event *e_fin, double& lambda,
           int& target, deque<double> &data, double &res, int &number, double &max_acc):sv_num(sv_num),
                                                          sv_len(sv_len),
                                                          e_ready(e_ready),
                                                          e_next(e_next),
                                                          e_fin(e_fin),
                                                          lambda(lambda),
                                                          target(target),
                                                          data(data),
                                                          res(res),
                                                          number(number),
                                                          max_acc(max_acc)
                                                          
{
   cout<<"Classificator constucted"<<endl;
   max_acc=0;
   for(int i=0; i<10; i++)
   {
      string str("Core_no_");
      string num=to_string(i);
      str=str+num;
      cores[i]=new Core(str.c_str(),sv_num, sv_len, &e_ready[i], &e_next[i], e_fin, lambda, target, data, res, max_acc);
   }
   SC_THREAD(classify);
}

void Classificator::classify()
{
   int k;
   double max;
   while(true)
   {  
      k=0;
      while(k<10)
      { 
         e_next[k].notify(SC_ZERO_TIME);
         wait(*e_fin);
         k++; 
      }

      max=cores[0]->get_acc();
      number=0;
      for(int i=1; i<10; i++)
      {
        if(max<cores[i]->get_acc())
        {
           max=cores[i]->get_acc();
           number=i;
        } 
      }
   }
   return;	
}

