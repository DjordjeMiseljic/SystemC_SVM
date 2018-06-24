#include "VP.hpp"
#include <iostream>


VP::VP(sc_module_name name) :
	sc_module(name),
	ic("Interconnect"),
	cl("Classificator"),
	mc("MControl"),
   de("Deskew"),
   br("BRAM")
{
	s_vp_t.register_b_transport(this, &VP::b_transport);

   cout<<name<<" constructed"<<endl;
	s_vp_i.bind(ic.s_ic_t);
   ic.s_ic_i0.bind(br.s_br_t0);
   ic.s_ic_i1.bind(de.s_de_t);
   ic.s_ic_i2.bind(cl.s_cl_t);
	cl.s_cl_i.bind(mc.s_mc_t);
   de.s_de_i.bind(br.s_br_t1);
   p_exp.bind(de.p_exp);
}

void VP::b_transport(pl_t& pl, sc_time& delay)
{
	s_vp_i->b_transport(pl, delay);
}
