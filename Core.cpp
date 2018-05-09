#include "Core.hpp"
#define P_CHECK_OVERFLOW if(p.overflow_flag()) cout<<BKG_YELLOW<<BLACK<<"WARNING"<<BKG_RST<<D_YELLOW<<" OVERFLOW DETECTED IN CORE"<<RST
#define R_CHECK_OVERFLOW if(res.overflow_flag()) cout<<BKG_YELLOW<<BLACK<<"WARNING"<<BKG_RST<<D_YELLOW<<" OVERFLOW DETECTED IN CORE"<<RST
#define A_CHECK_OVERFLOW if(acc.overflow_flag()) cout<<BKG_YELLOW<<BLACK<<"WARNING"<<BKG_RST<<D_YELLOW<<" OVERFLOW DETECTED IN CORE"<<RST


Core::Core(sc_module_name name, int& sv_num, int sv_len, 
           sc_event *e_ready, sc_event *e_next, sc_event *e_fin, lin_t& lambda,
           bin_t& bias, deque<din_t> &data, res_t &res):sv_num(sv_num),
                                                          sv_len(sv_len),
                                                          e_ready(e_ready),
                                                          e_next(e_next),
                                                          e_fin(e_fin),
                                                          lambda(lambda),
                                                          bias(bias),
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
   bin_t b;
   while(true)
   {
      wait(*e_ready);
      k=sv_num;
      b=bias;
      acc=0.0;
      y.clear();
      for(int i=0;i<sv_len;i++)
         y.push_back(data[i]);

      while(k)
         {
            e_next->notify(SC_ZERO_TIME);
            wait(*e_ready);
            p=1.0;
            for(int i=0; i<sv_len; i++)
               p+=y[i].to_double()*data[i].to_double();


            p*=0.1;
            P_CHECK_OVERFLOW<<" mult 0.1"<<endl;

            p=p*p*p;
            P_CHECK_OVERFLOW<<" cube "<<endl;

            p=p*lambda.to_double();
            P_CHECK_OVERFLOW<<" lambda*p"<<endl;

            wait(1,SC_NS);
            
            //cout<<"\tcurrent acc="<<acc<<"\tp="<<p<<"\tnew acc="<<(acc+p);
            //cout<<"\t@"<<sc_time_stamp()<<"\t#"<<name()<<endl;
            
            acc+=p.to_double();
            A_CHECK_OVERFLOW<<" acc+=p"<<endl;
            k--;
         }
      acc+=b.to_double();
      A_CHECK_OVERFLOW<<"acc+=b"<<endl;
      res=acc;
      R_CHECK_OVERFLOW<<"res=acc"<<endl;

      e_fin->notify(SC_ZERO_TIME);
   }
   return;	
}

acc_t Core::get_acc()
{
   return acc;
}
