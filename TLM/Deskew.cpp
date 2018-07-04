#ifndef DESKEW_C
#define DESKEW_C
#include "Deskew.hpp"

Deskew::Deskew(sc_module_name name):sc_module(name)
{
   s_de_t.register_b_transport(this, &Deskew::b_transport);
   cout<<name<<" constructed"<<endl;
   SC_THREAD(proc);
   toggle = SC_LOGIC_0;
   start = SC_LOGIC_0;
   image.reserve(SV_LEN);
   
}

void Deskew::proc()
{
   pl_t pl;
   tlm_command cmd    = pl.get_command();
   uint64 adr         = pl.get_address();
   unsigned char *buf = pl.get_data_ptr();
   sc_time offset=SC_ZERO_TIME;
   #ifdef QUANTUM
   tlm_utils::tlm_quantumkeeper qk;
   qk.reset();
   #endif
   while(1)
   {
      //cout<<"DSKW: WAITING FOR START"<<endl;
      while(start==SC_LOGIC_0)
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

      image.clear();
      //cout<<"DSKW: READ IMAGE FROM BRAM"<<endl;
      //READ IMAGE FROM BRAM 
      pl.set_command(TLM_READ_COMMAND);
      pl.set_address(0x80000000);
      pl.set_data_length(SV_LEN);

      s_de_i -> b_transport(pl, offset);
      assert(pl.get_response_status() == TLM_OK_RESPONSE);

      #ifdef QUANTUM
      qk.inc(sc_time(10, SC_NS));
      offset = qk.get_local_time();
      qk.set_and_sync(offset);
      #else
      offset += sc_time(50, SC_NS);
      #endif

     // cout<<"DSKW: DESKEWING"<<endl;
      //DESKEW IT
      buf = pl.get_data_ptr();
      for(int i=0; i<SV_LEN; i++)
         image.push_back(((din_t*)buf)[i]);
      image = deskew(image);
      
      #ifdef QUANTUM
      qk.inc(sc_time(10, SC_NS));
      offset = qk.get_local_time();
      qk.set_and_sync(offset);
      #else
      offset += sc_time(10, SC_NS);
      #endif
      
      //cout<<"DSKW: WRITE TO BRAM"<<endl;
      //WRITE NEW IMAGE TO BRAM
      pl.set_data_ptr((unsigned char *)&image[0]);
      pl.set_command(TLM_WRITE_COMMAND);
      pl.set_address(0x80000000+SV_LEN);
      pl.set_data_length(SV_LEN);

      s_de_i -> b_transport(pl, offset);
      assert(pl.get_response_status() == TLM_OK_RESPONSE);

      //cout<<"DSKW: SET TOGGLE"<<endl;
      toggle = SC_LOGIC_1; 
      p_out->write(toggle);//finished, send interrupt
      
      #ifdef QUANTUM
      qk.inc(sc_time(10, SC_NS));
      offset = qk.get_local_time();
      qk.set_and_sync(offset);
      #else
      offset += sc_time(20, SC_NS);
      #endif
      
      //cout<<"DSKW: RESET TOGGLE"<<endl;
      toggle = SC_LOGIC_0; 
      p_out->write(toggle);//finished, send interrupt

   }
}
void Deskew::b_transport(pl_t& pl, sc_time& offset)
{
   tlm_command cmd    = pl.get_command();
   uint64 adr         = pl.get_address();
   unsigned char *buf = pl.get_data_ptr();
   
   if(cmd==TLM_WRITE_COMMAND && adr==0x81000000)
   {
      start=SC_LOGIC_1; 
      pl.set_response_status(TLM_OK_RESPONSE);
      offset += sc_time(10, SC_NS);
      
   }
   else if(cmd==TLM_READ_COMMAND && adr==0x81000000)
   {

      toggle = SC_LOGIC_0; 
      p_out->write(toggle);//finished, send interrupt
      pl.set_response_status(TLM_OK_RESPONSE);
   }
   else 
   {
      pl.set_response_status(TLM_COMMAND_ERROR_RESPONSE);
   } 

}





vector<din_t> Deskew:: deskew(vector <din_t> image)
{
   p_t mu02 = 0;
   p_t mu11 = 0;
   
   vector <din_t> new_image;
   deskew_t skew = 0;
   array<array<deskew_t, 3>, 2> M;
   deskew_t xp, yp,R1,R2;
   din_t P;
   deskew_t x1,y1,x2,y2;
   new_image.reserve(784);

   calc_moments(image, mu11, mu02);
   skew = mu11/mu02;
   if(skew.overflow_flag())
      cout<<"SKEW_OVERFLOW"<<endl;
   
   
   M[0][0] = 1;
   M[0][1] = skew;
   M[0][2] = -0.5*28*skew;
   if(M[0][2].overflow_flag())
      cout<<"M[0][2] OVERFLOW"<<endl;
   M[1][0] = 0;
   M[1][1] = 1;
   M[1][2] = 0;
   
   for (int x=0; x<28; x++)
      for(int y = 0; y<28; y++)
         {
            xp = (M[0][0]*x + M[0][1]*y + M[0][2]);
            if(xp.overflow_flag())
               cout<<BKG_YELLOW<<"XP OVERFLOW"<<BKG_RST<<endl;
            yp = (M[1][0]*x + M[1][1]*y + M[1][2]);
            if(yp.overflow_flag())
               cout<<BKG_YELLOW<<"YP OVERFLOW"<<BKG_RST<<endl;            
            if(xp<27 && yp<27 && xp>= 0 && yp >=0)
            {
               x1=(int)xp;
               y1=(int)yp;
               x2=(x1+1);
               y2=(y1+1);
               R1 = image[x1+y1*28] + (xp-x1)/(x2-x1)*(image[x2+y1*28]-image[x1+y1*28]);
               R2 = image[x1+y2*28] + (xp-x1)/(x2-x1)*(image[x2+y2*28]-image[x1+y2*28]);

               P = R2 + (yp-y1)/(y2-y1)*(R1-R2);
               
                if(P.overflow_flag())
                   cout<<"overflow at P"<<endl;

               if(P>1)
                  cout<<"error"<<endl;

               new_image[x+y*28]=P;
              
            }
            else
               new_image[x+y*28]=0;
         }
   
   
   return new_image;
}


void Deskew::calc_moments(vector<din_t> image, p_t& mu11, p_t& mu02)
{
   p_t m00,m10,m01 = 0;
   p_t x_mc, y_mc;
   p_t max = 0;
   
   for (int x=0; x<28; x++)
      for(int y = 0; y<28; y++)
         {
            m00 += image[x+y*28];
            if(m00.overflow_flag())
               cout<<"m00 overflow"<<endl;

            m10 += image[x+y*28]*x;
            if(m10.overflow_flag())
               cout<<"m10 overflow"<<endl;

            m01 += image[x+y*28]*y;
            if(m01.overflow_flag())
               cout<<"m10 overflow"<<endl;
            
         }
   
   x_mc = m10/m00;
   y_mc = m01/m00;
   for (int x=0; x<28; x++)
      for(int y = 0; y<28; y++)
         {
            
            mu02 += (image[x+y*28]*pow((y-y_mc),2));
            if(mu02.overflow_flag())
               cout<<"mu02 overflow:"<<mu02<<endl;
            
            mu11 += (image[x+y*28]*(x-x_mc)*(y-y_mc));
            if(mu11.overflow_flag())
               cout<<"mu11 overflow:"<<mu11<<endl;
         }
   
   
   
}


#endif
