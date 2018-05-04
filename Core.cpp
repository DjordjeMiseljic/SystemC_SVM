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
   int k;
   long double p;
   double b;
   double tmp;
   e_next->notify(SC_ZERO_TIME);
   wait(*e_ready);
   k=sv_num;
   b=lambda;
   cout<<"BIAS:"<<b<<endl;
   //copy inputs to y 
   //cout<<"Y: ";
   for(int i=0;i<sv_len;i++)
   {
      y.push_back(data[i]);
      //cout<<"\t"<<data[i];
   }
   cout<<endl;
   while(k)
      {
         cout<<"for k= "<<k<<" : "<<endl;
         e_next->notify(SC_ZERO_TIME);
         wait(*e_ready);
         //cout<<"tmp=";
         p=1.0;
         for(int i=0; i<sv_len; i++)
         {
            tmp=y[i]*data[i];
            p+=tmp;
            //cout<<" "<<y[i];
         }
         cout<<"\n   pdot:"<<p;
         
         p=p*p*p;
         cout<<"   pcube:"<<p;
         
         p=(lambda)*p;
         cout<<"   plambda:"<<p<<endl;
         
         p=(target)*p;
        
         wait(1,SC_NS);
         
         cout<<"\tcurrent acc="<<acc<<"\tp="<<p<<"\tnew acc="<<(acc+p);
         cout<<"\t@"<<sc_time_stamp()<<"\t#"<<name()<<endl;
         
         acc+=p;
         k--;
      }
   acc+=b;
   cout<<"classification finished\nres= "<<acc<<"\nreal_res= "<<res<<endl;
   return;	
}

