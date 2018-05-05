#include "Core.hpp"

Core::Core(sc_module_name name, int& sv_num, int sv_len, 
           sc_event *e_ready, sc_event *e_next, sc_event *e_fin, double& lambda,
           int& target, deque<double> &data, double &res):sv_num(sv_num),
                                                          sv_len(sv_len),
                                                          e_ready(e_ready),
                                                          e_next(e_next),
                                                          e_fin(e_fin),
                                                          lambda(lambda),
                                                          target(target),
                                                          data(data),
                                                          res(res)
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
      //cout<<"next notify <-------\t#"<<name()<<endl;
      //e_next->notify(SC_ZERO_TIME);
      //cout<<"wait rdy\t#"<<name()<<endl;
      wait(*e_ready);
      k=sv_num;
      b=lambda;
      acc=0;
      y.clear();
      for(int i=0;i<sv_len;i++)
         y.push_back(data[i]);

      while(k)
         {
            //cout<<"for k= "<<k<<" : "<<endl;
           // cout<<"next notify <-------\t#"<<name()<<endl;
            e_next->notify(SC_ZERO_TIME);
           // cout<<"wait rdy\t#"<<name()<<endl;
            wait(*e_ready);
            //cout<<"dlen="<<data.size()<<"ylen"<<y.size()<<endl;
            p=1;
            for(int i=0; i<sv_len; i++)
               p+=y[i]*data[i];
            
            /* if(i%7==0)
                  cout<<endl;
               cout<<" "<<data[i];*/
            p=p*p*p;
            //cout<<"   pcube:"<<p;
            
            p=lambda*p;
            
            p=target*p;
           
            wait(1,SC_NS);
            
            //cout<<"\tcurrent acc="<<acc<<"\tp="<<p<<"\tnew acc="<<(acc+p);
            //cout<<"\t@"<<sc_time_stamp()<<"\t#"<<name()<<endl;
            
            acc+=p;
            k--;
         }
      acc+=b;
      //cout<<"fin notify ********\t#"<<name()<<endl;
      e_fin->notify(SC_ZERO_TIME);
      cout<<"classification finished:\tres= "<<acc<<"\t["<<res<<"]";
      cout<<"\t@"<<sc_time_stamp()<<"\t#"<<name()<<endl;
   }
   return;	
}

double Core::get_acc()
{
   return acc;
}
