#include "Core.hpp"


Core::Core(sc_module_name name, int& sv_num, int sv_len, 
           sc_event *e_ready, sc_event *e_next, sc_event *e_fin, din_t& lambda,
           din_t& target, deque<din_t> &data, din_t &res, double &max_acc):sv_num(sv_num),
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

double Core::maxAcc=0;

void Core::proc()
{
   int k;
   p_t p;
   din_t b;
   while(true)
   {
      wait(*e_ready);
      k=sv_num;
      b=lambda;//b is sent trough lambda on first transaction
      acc=0;
      y.clear();
      for(int i=0;i<sv_len;i++)
         y.push_back(data[i]);

      while(k)
         {
            e_next->notify(SC_ZERO_TIME);
            wait(*e_ready);
            p=1.0;
            for(int i=0; i<sv_len; i++)
               p+=y[i]*data[i];

            if(p.to_double()>max_acc)
            {
              max_acc=p.to_double(); 
              //cout<<"P1"<<endl;
            }

            p*=0.1;
            p=p*p*p;

            if(p.to_double()>max_acc)
            {
              max_acc=p.to_double(); 
              //cout<<"P2"<<endl;
            }

            p=lambda*p;

            if(p.to_double()>max_acc)
            {
              max_acc=p.to_double(); 
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
      if (maxAcc<acc.to_double())
      {
         maxAcc=acc.to_double();
         cout<<"new maxAcc = "<<maxAcc<<endl;
      }
      res=acc;
      if(res.overflow_flag())
        cout<<"Overflow detected!"<<endl; 
      //cout<<"single core classification finished:\tres= "<<acc<<"\t["<<res<<"]"<<endl;
      e_fin->notify(SC_ZERO_TIME);
      //cout<<"\t@"<<sc_time_stamp()<<"\t#"<<name()<<endl;
   }
   return;	
}

acc_t Core::get_acc()
{
   return acc;
}
