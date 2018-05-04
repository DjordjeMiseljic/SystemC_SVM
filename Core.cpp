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
   double p;

   e_next->notify(SC_ZERO_TIME);
   wait(*e_ready);
   k=sv_num;
   //copy inputs to y 
   for(int i=0;i<sv_len;i++)
      y.push_back(data[i]);
   while(k)
      {
         cout<<"for k= "<<k<<" : "<<endl;
         p=1;
         e_next->notify(SC_ZERO_TIME);
         wait(*e_ready);
         for(int i=0; i<sv_len; i++)
            p+=(y[i]*data[i]);
         cout<<"\tpdot:"<<p;
         p=p*p*p;
         cout<<"\tpcube"<<p;
         p=(lambda)*p;
         cout<<"\tplambda"<<p<<endl;
         p=(target)*p;

         wait(1,SC_NS);
         
         printf("   current acc= %1.5e    p= %1.5e    new acc= %1.5e    ",acc,p,(acc+p));
         cout<<"@"<<sc_time_stamp()<<"\t#"<<name()<<endl;
         //cout<<"\tcurrent acc= "<<acc<<"\tp= "<<p<<"\t@ "<<sc_time_stamp()<<"\t#"<<name()<<endl;
         
         acc+=p;
         k--;
      }
   cout<<"classification finished\nres= "<<acc<<"\nreal_res= "<<res<<endl;
   return;	
}

