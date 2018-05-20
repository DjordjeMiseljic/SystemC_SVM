#include "VP.hpp"
#include <iostream>


VP::VP(sc_module_name name) :
	sc_module(name),
	ic("Interconnect"),
	cl("Classificator"),
	mc("MControl"),
        de("Deskew")
{
	s_vp_t.register_b_transport(this, &VP::b_transport);

        cout<<name<<" constructed"<<endl;
	s_vp_i.bind(ic.s_ic_t);
        ic.s_ic_i.bind(de.s_de_t);
	de.s_de_i.bind(cl.s_cl_t);
	cl.s_cl_i.bind(mc.s_mc_t);
}

void VP::b_transport(pl_t& pl, sc_time& delay)
{
	s_vp_i->b_transport(pl, delay);
}
