#include "VP.hpp"
#include <iostream>


VP::VP(sc_module_name name) :
	sc_module(name),
	ic("Interconnect"),
	cl("Classificator"),
   de("Deskew"),
   br("BRAM"),

	mc("MControl"),
   dm("DMA"),
   fifo(785)
{

	s_vp_t0.register_b_transport(this, &VP::b_transport0);
	s_vp_t1.register_b_transport(this, &VP::b_transport1);

   cout<<name<<" constructed"<<endl;
	s_vp_i0.bind(ic.s_ic_t);
	s_vp_i1.bind(mc.s_mc_t1);
   ic.s_ic_i0.bind(br.s_br_t0);
   ic.s_ic_i1.bind(de.s_de_t);
   ic.s_ic_i2.bind(cl.s_cl_t);
   ic.s_ic_i3.bind(dm.s_dm_t);
   de.s_de_i.bind(br.s_br_t1);
	dm.s_dm_i.bind(mc.s_mc_t0);

   dm.p_fifo(fifo);
   cl.p_fifo(fifo);

   de.p_out(p_out0);
   cl.p_out(p_out1);

}

void VP::b_transport0(pl_t& pl, sc_time& delay)
{
	s_vp_i0->b_transport(pl, delay);
}
void VP::b_transport1(pl_t& pl, sc_time& delay)
{
	s_vp_i1->b_transport(pl, delay);
}
