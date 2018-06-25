#include "VP.hpp"
#include <iostream>


VP::VP(sc_module_name name) :
	sc_module(name),
	ic("Interconnect"),
	cl("Classificator"),
	mc("MControl"),
   de("Deskew"),
   br("BRAM"),
   dm("DMI"),
   fifo(784)
{

	s_vp_t.register_b_transport(this, &VP::b_transport);

   cout<<name<<" constructed"<<endl;
	s_vp_i.bind(ic.s_ic_t);
   ic.s_ic_i0.bind(br.s_br_t0);
   ic.s_ic_i1.bind(de.s_de_t);
   ic.s_ic_i2.bind(cl.s_cl_t);
   ic.s_ic_i3.bind(dm.s_dm_t);
	dm.s_dm_i.bind(mc.s_mc_t);
   de.s_de_i.bind(br.s_br_t1);

   dm.p_fifo(fifo);
   cl.p_fifo(fifo);

   p_exp0.bind(de.p_exp);
   p_exp1.bind(cl.p_exp);

}

void VP::b_transport(pl_t& pl, sc_time& delay)
{
	s_vp_i->b_transport(pl, delay);
}
