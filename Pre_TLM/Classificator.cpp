#include "Classificator.hpp"
Classificator::Classificator(sc_module_name name, int& sv_num, int sv_len, 
           sc_event *e_ready, sc_event *e_next, sc_event *e_fin, lin_t &lambda,
           bin_t &bias, deque<din_t> &data, res_t &res, num_t &number):sv_num(sv_num),
                                                          sv_len(sv_len),
                                                          e_ready(e_ready),
                                                          e_next(e_next),
                                                          e_fin(e_fin),
                                                          lambda(lambda),
                                                          bias(bias),
                                                          data(data),
                                                          res(res),
                                                          number(number)
                                                          
{
   cout<<"Classificator constucted"<<endl;
   for(int i=0; i<10; i++)
   {
      string str("Core_no_");
      string num=to_string(i);
      str=str+num;
      cores[i]=new Core(str.c_str(),sv_num, sv_len, &e_ready[i], &e_next[i], e_fin, lambda, bias, data, res);
   }
   SC_THREAD(classify);
}

void Classificator::classify()
{
   int k;
   res_t max;
   while(true)
   {  
      k=0;
      while(k<10)
      { 
         e_next[k].notify(SC_ZERO_TIME);
         wait(*e_fin);
         k++; 
      }

      max=cores[0]->get_acc();
      number=0;
      for(int i=1; i<10; i++)
      {
        if(max<cores[i]->get_acc())
        {
           max=cores[i]->get_acc();
           number=i;
        } 
      }
   }
   return;	
}

