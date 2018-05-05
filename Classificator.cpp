#include "Classificator.hpp"
Classificator::Classificator(sc_module_name name, int& sv_num, int sv_len, 
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
   cout<<"Classificator constucted"<<endl;
   for(int i=0; i<10; i++)
   {
      string str("core_no_");
      string num=to_string(i);
      str=str+num;
      cores[i]=new Core(str.c_str(),sv_num, sv_len, &e_ready[i], &e_next[i], &e_fin, lambda, target, data, res);
   }
   SC_THREAD(classify);
}

void Classificator::classify()
{
   int k;
   double max;
   int number;
   while(true)
   {  
      k=10;
      while(k)
      { 
         wait(e_fin);
         k--; 
      }

      max=cores[0]->get_acc();
      number=0;
      for(int i=1; i<10; i++)
      {
        if(max>cores[i]->get_acc())
        {
           max=cores[i]->get_acc();
           number=i;
        } 
      }
      cout<<"Number is: "<<number<<"\t@"<<sc_time_stamp()<<"\t#"<<name()<<endl;
   }
   return;	
}

