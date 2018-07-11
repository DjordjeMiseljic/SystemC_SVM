#ifndef CLASSIFICATOR_C
#define CLASSIFICATOR_C
#include "Classificator.hpp"

#define SV0 0
#define SV1 (361*(SV_LEN+1))+1
#define SV2 SV1+(267*(SV_LEN+1))+1
#define SV3 SV2+(581*(SV_LEN+1))+1
#define SV4 SV3+(632*(SV_LEN+1))+1
#define SV5 SV4+(480*(SV_LEN+1))+1
#define SV6 SV5+(513*(SV_LEN+1))+1
#define SV7 SV6+(376*(SV_LEN+1))+1
#define SV8 SV7+(432*(SV_LEN+1))+1
#define SV9 SV8+(751*(SV_LEN+1))+1

const array<int, 10> sv_start_addr = {SV0, SV1, SV2, SV3, SV4, SV5, SV6, SV7, SV8, SV9};

Classificator::Classificator(sc_module_name name): sc_module(name)
{
   s_cl_t.register_b_transport(this, &Classificator::b_transport);
   cout<<name<<" constucted"<<endl;
   SC_THREAD(classify);
   
   image_v.reserve(SV_LEN);
   res_v.reserve(10);
   toggle = SC_LOGIC_0;
   start = SC_LOGIC_0;
}

void Classificator::b_transport(pl_t& pl, sc_time& offset)
{
	
   tlm_command cmd    = pl.get_command();
   uint64 adr         = pl.get_address();
   unsigned char *buf = pl.get_data_ptr();
   unsigned int len   = pl.get_data_length();

   switch(cmd)
   {
   case TLM_WRITE_COMMAND://Begin classification
         
      image_v.clear();
      pl.set_response_status( TLM_OK_RESPONSE );
      start=SC_LOGIC_1;
      offset+=sc_time(10, SC_NS);
      break;

   case TLM_READ_COMMAND://Read classified number 
         
      buf=(unsigned char *)&cl_num;
      pl.set_data_ptr        ( buf );
      pl.set_response_status( TLM_OK_RESPONSE );
      offset+=sc_time(5, SC_NS);
      break;

   default:
      pl.set_response_status( TLM_COMMAND_ERROR_RESPONSE );
   }

}


void Classificator::classify ()
{
   din_t fifo_tmp;
   sc_time offset=SC_ZERO_TIME;
   #ifdef QUANTUM
   tlm_utils::tlm_quantumkeeper qk;
   qk.reset();
   #endif

   while(1)
   {
      while(start==SC_LOGIC_0)//wait for start reg
      {
         #ifdef QUANTUM
         qk.inc(sc_time(10, SC_NS));
         offset = qk.get_local_time();
         qk.set_and_sync(offset);
         #else
         offset += sc_time(10, SC_NS);
         #endif
         
      }
      start=SC_LOGIC_0;
      toggle =  SC_LOGIC_1; 
      p_out->write(toggle);// wait
      for( int p=0; p<SV_LEN; p++)
      {
         //send interrupt and then waste time until there is something in fifo
         //waiting for test image in fifo
         while(!p_fifo->nb_read(fifo_tmp))
         {
            #ifdef QUANTUM
            qk.inc(sc_time(10, SC_NS));
            offset = qk.get_local_time();
            qk.set_and_sync(offset);
            #else
            offset += sc_time(10, SC_NS);
            #endif
         }
         image_v[p]= fifo_tmp;
         toggle =  SC_LOGIC_0; 
         p_out->write(toggle);
      }

      
      res_v.clear();
      for(unsigned int core=0; core<10; core++)
      {
         acc=0;

         for( int sv=0; sv<sv_array[core]; sv++)
         {
            p=1.0;
            toggle =  SC_LOGIC_1; 
            p_out->write(toggle);
            for( int i=0; i<SV_LEN; i++)
            {
               //send interrupt and then waste time until there is something in fifo
               //waiting for support vectors in fifo
               while(!p_fifo->nb_read(fifo_tmp))
               {
                  #ifdef QUANTUM
                  qk.inc(sc_time(10, SC_NS));
                  offset = qk.get_local_time();
                  qk.set_and_sync(offset);
                  #else
                  offset += sc_time(10, SC_NS);
                  #endif
               }
               p+=image_v[i]*fifo_tmp;
               toggle =  SC_LOGIC_0; 
               p_out->write(toggle);
            }
            p*=0.1;
            p=p*p*p;
               
            toggle =  SC_LOGIC_1; 
            p_out->write(toggle);
            //send interrupt and then waste time until there is something in fifo
            //waiting for lambda in fifo
            while(!p_fifo->nb_read(fifo_tmp))
            {
               #ifdef QUANTUM
               qk.inc(sc_time(10, SC_NS));
               offset = qk.get_local_time();
               qk.set_and_sync(offset);
               #else
               offset += sc_time(10, SC_NS);
               #endif
            }
            toggle =  SC_LOGIC_0; 
            p_out->write(toggle);
            
            p*= fifo_tmp;
            acc+=p;
         }

         toggle =  SC_LOGIC_1; 
         p_out->write(toggle);// wait
         //send interrupt and then waste time until there is something in fifo
         //waiting for bias in fifo
         while(!p_fifo->nb_read(fifo_tmp))
         {
            #ifdef QUANTUM
            qk.inc(sc_time(10, SC_NS));
            offset = qk.get_local_time();
            qk.set_and_sync(offset);
            #else
            offset += sc_time(10, SC_NS);
            #endif
         }
         toggle =  SC_LOGIC_0; 
         p_out->write(toggle);

         acc+=fifo_tmp;
         res_v.push_back (acc);
         
      }
      //find most convincing result of 10 cores
      max_res=res_v[0];
      cl_num=0;
      for(int i=1; i<10; i++)
      {
         if(max_res<res_v[i])
         {
            max_res=res_v[i];
            cl_num=(num_t)i;
         }
      }
      //classification of test image finished, send interrupt to test bench
      toggle =  SC_LOGIC_1; 
      p_out->write(toggle);// wait
      #ifdef QUANTUM
      qk.inc(sc_time(20, SC_NS));
      offset = qk.get_local_time();
      qk.set_and_sync(offset);
      #endif
      toggle =  SC_LOGIC_0; 
      p_out->write(toggle);
   }
}
#endif
