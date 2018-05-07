#include "Core.hpp"

Core::Core(sc_module_name name, int& sv_num, int sv_len, 
           sc_event *e_ready, sc_event *e_next, sc_event *e_fin, double& lambda,
           int& target, deque<double> &data, double &res, double &max_acc):sv_num(sv_num),
                                                          sv_len(sv_len),
                                                          e_ready(e_ready),
                                                          e_next(e_next),
                                                          e_fin(e_fin),
                                                          lambda(lambda),
                                                          target(target),
                                                          data(data),
                                                          res(res),
                                                          max_acc(max_acc)
{
   cout<<name<<" constucted"<<endl;
   SC_THREAD(proc);
}

void Core::proc()
{
   int k;
   double p;
   double b;
   while(true)
   {
      wait(*e_ready);
      k=sv_num;
      b=lambda;
      acc=0;
      y.clear();
      for(int i=0;i<sv_len;i++)
         y.push_back(data[i]);

      while(k)
         {
            e_next->notify(SC_ZERO_TIME);
            wait(*e_ready);
            p=1;
            for(int i=0; i<sv_len; i++)
               p+=y[i]*data[i];

            if(p>max_acc)
            {
              max_acc=p; 
              //cout<<"P1"<<endl;
            }

            p/=10;
            p=p*p*p;

            if(p>max_acc)
            {
              max_acc=p; 
              //cout<<"P2"<<endl;
            }

            p=lambda*p;
            p*=1000;

            if(p>max_acc)
            {
              max_acc=p; 
              //cout<<"P3"<<endl;
            }

            p=target*p;
           
            wait(1,SC_NS);
            
            //cout<<"\tcurrent acc="<<acc<<"\tp="<<p<<"\tnew acc="<<(acc+p);
            //cout<<"\t@"<<sc_time_stamp()<<"\t#"<<name()<<endl;
            
            acc+=p;
            k--;
         }
      acc+=b;
      res=acc;
      //cout<<"single core classification finished:\tres= "<<acc<<"\t["<<res<<"]"<<endl;
      e_fin->notify(SC_ZERO_TIME);
      //cout<<"\t@"<<sc_time_stamp()<<"\t#"<<name()<<endl;
   }
   return;	
}

double Core::get_acc()
{
   return acc;
}
