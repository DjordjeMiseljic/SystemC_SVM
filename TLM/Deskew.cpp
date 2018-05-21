#ifndef DESKEW_C
#define DESKEW_C
#include "Deskew.hpp"

Deskew::Deskew(sc_module_name name):sc_module(name)
{
   s_de_t.register_b_transport(this, &Deskew::b_transport);
   cout<<name<<" constructed"<<endl;
   
}

void Deskew::b_transport(pl_t& pl, sc_time& offset)
{
   tlm_command cmd    = pl.get_command();
   uint64 adr         = pl.get_address();
   unsigned char *buf = pl.get_data_ptr();
   vector<din_t> image;
   image.reserve(SV_LEN);
   switch (cmd)
   {
      case TLM_WRITE_COMMAND:
         for(int i=0; i<SV_LEN; i++)
            image.push_back(((din_t*)buf)[i]);
         image = deskew(image);
         pl.set_data_ptr((unsigned char*)&image[0]);
         offset += sc_time(5, SC_NS);
         
         s_de_i->b_transport(pl,offset);
         break;
      case TLM_READ_COMMAND:
         offset += sc_time(1, SC_NS);
         s_de_i->b_transport(pl, offset);
         break;
      default:
         pl.set_response_status( TLM_COMMAND_ERROR_RESPONSE );
   }
}


vector<din_t> Deskew:: deskew(vector <din_t> image)
{
   p_t mu02 = 0;
   p_t mu11 = 0;
   array<vector<din_t>, 28> reshaped_image, new_image_28;
   deskew_t skew = 0;
   array<array<deskew_t, 3>, 2> M;
   deskew_t xp, yp,R1,R2;
   din_t P;
   deskew_t x1,y1,x2,y2;


   calc_moments(image, mu11, mu02);
   skew = mu11/mu02;
   if(skew.overflow_flag())
      cout<<"SKEW_OVERFLOW"<<endl;
   
   reshaped_image = reshape_image_28(image);
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
               R1 = reshaped_image[y1][x1] + (xp-x1)/(x2-x1)*(reshaped_image[y1][x2]-reshaped_image[y1][x1]);
               
               R2 = reshaped_image[y2][x1] + (xp-x1)/(x2-x1)*(reshaped_image[y2][x2]-reshaped_image[y2][x1]);

               P = R2 + (yp-y1)/(y2-y1)*(R1-R2);
               
                if(P.overflow_flag())
                   cout<<"overflow at P"<<endl;

               if(P>1)
                  cout<<"error"<<endl;

               new_image_28[y].push_back(P);
              
            }
            else
               new_image_28[y].push_back(0);
         }
   
   image = reshape_image_784(new_image_28);
   return image;
}


void Deskew::calc_moments(vector<din_t> image, p_t& mu11, p_t& mu02)
{
   p_t m00,m10,m01 = 0;
   p_t x_mc, y_mc;
   p_t max = 0;
   array<vector<din_t>, 28> reshaped_image;
   reshaped_image = reshape_image_28(image);
   for (int x=0; x<28; x++)
      for(int y = 0; y<28; y++)
         {
            m00 += reshaped_image[y][x];
            if(m00.overflow_flag())
               cout<<"m00 overflow"<<endl;
            m10 += reshaped_image[y][x]*x;
            if(m10.overflow_flag())
               cout<<"m10 overflow"<<endl;
            m01 += reshaped_image[y][x]*y;
            if(m01.overflow_flag())
               cout<<"m10 overflow"<<endl;
            
         }
   
   x_mc = m10/m00;
   y_mc = m01/m00;
   for (int x=0; x<28; x++)
      for(int y = 0; y<28; y++)
         {
            mu02 += (reshaped_image[y][x]*pow((y-y_mc),2));
            if(mu02.overflow_flag())
               cout<<"mu02 overflow:"<<mu02<<endl;
            mu11 += (reshaped_image[y][x]*(x-x_mc)*(y-y_mc));
            if(mu11.overflow_flag())
               cout<<"mu11 overflow:"<<mu11<<endl;
         }
   //cout<<"mu02: "<<mu02<<"\t mu11: "<<mu11<<endl;
   
   
}
array<vector<din_t>, 28> Deskew:: reshape_image_28(vector<din_t> image)
{
   int k = 0;
   array<vector<din_t>, 28> new_image;
   for (int i = 0; i < 784; ++i)
      {
         if(i%28 == 0)
         {
            if(i != 0)
               k++;
         }
         new_image[k].push_back(image[i]);

      }
   
   return new_image;
}

vector<din_t> Deskew::reshape_image_784(array<vector<din_t>, 28> image)
{
   int k=0,j=0;
   vector<din_t> new_image;
   for (int i = 0; i < 784; ++i)
   {
      
      if(i%28 == 0)
      {
         if(i != 0)
            k++;
         j = 0;
      }
      new_image.push_back(image[k][j]);
      j++;

      
   }

   return new_image;
}


#endif
