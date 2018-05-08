#include "Core.hpp"
#define P_CHECK_OVERFLOW if(p.overflow_flag()) cout<<BKG_YELLOW<<BLACK<<"WARNING"<<BKG_RST<<D_YELLOW<<" OVERFLOW DETECTED IN CORE"<<RST<<endl;
#define A_CHECK_OVERFLOW if(acc.overflow_flag()) cout<<BKG_YELLOW<<BLACK<<"WARNING"<<BKG_RST<<D_YELLOW<<" OVERFLOW DETECTED IN CORE"<<RST<<endl;


Core::Core(sc_module_name name, int& sv_num, int sv_len, 
           sc_event *e_ready, sc_event *e_next, sc_event *e_fin, din_t& lambda,
           din_t& target, deque<din_t> &data, acc_t &res):sv_num(sv_num),
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

            P_CHECK_OVERFLOW

            p*=0.1;
            p=p*p*p;

            P_CHECK_OVERFLOW

            p=lambda*p;

            P_CHECK_OVERFLOW

            p=target*p;
           
            wait(1,SC_NS);
            
            //cout<<"\tcurrent acc="<<acc<<"\tp="<<p<<"\tnew acc="<<(acc+p);
            //cout<<"\t@"<<sc_time_stamp()<<"\t#"<<name()<<endl;
            
            acc+=p;
            k--;
         }
      acc+=b;
      res=acc;
      A_CHECK_OVERFLOW
      e_fin->notify(SC_ZERO_TIME);
   }
   return;	
}

acc_t Core::get_acc()
{
   return acc;
}
