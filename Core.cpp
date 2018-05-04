#include "Core.hpp"

Core::Core(sc_module_name name, int& sv_num, int sv_len, 
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
   cout<<"Core constucted"<<endl;
   SC_THREAD(proc);
   acc=0;
}

void Core::proc()
{
   
   long double p;

   e_next->notify(SC_ZERO_TIME);

   wait(*e_ready);
   int k=sv_num;
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
         p=(target)*p;
         acc+=p;

         wait(1,SC_NS);
         cout<<"p= "<<p<<"\tacc= "<<acc<<"\treal res is: "<<res<<"\t@ "<<sc_time_stamp()<<"\t#"<<name()<<endl;
         cout<<"k is: "<<k<<endl;
         k--;
      }
   //cout<<"classification finished\nacc= "<<acc<<endl;
   return;	
}

