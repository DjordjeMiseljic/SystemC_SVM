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

double Core::max_res=0;
double Core::maxAcc=0;

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

            if(abs(p)>max_acc)
            {
               max_acc=p; 
               //cout<<"new maximum p="<<max_acc<<endl; 
            }

            p*=0.1;
            p=p*p*p;

            if(abs(p)>max_acc)
            {
               max_acc=abs(p);
               //cout<<"new maximum p="<<max_acc<<endl; 
            }

            p=lambda*p;
            p*=1000;

            if(abs(p)>max_acc)
            {
               max_acc=abs(p); 
               //cout<<"new maximum p="<<max_acc<<endl; 
            }

            p=target*p;
           
            wait(1,SC_NS);
            
            //cout<<"\tcurrent acc="<<acc<<"\tp="<<p<<"\tnew acc="<<(acc+p);
            //cout<<"\t@"<<sc_time_stamp()<<"\t#"<<name()<<endl;
            
            acc+=p;
           /* if(maxAcc<abs(acc))
            {
               maxAcc=abs(acc);
               cout<<"#### New MAX ACC: "<<maxAcc<<endl; 
            }*/
            k--;
         }
      acc+=b;
      res=acc;
      /*if(max_res<abs(res))
      {
         max_res=abs(res);
         cout<<"#### New MAX RES: "<<max_res<<endl; 
      }*/
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
