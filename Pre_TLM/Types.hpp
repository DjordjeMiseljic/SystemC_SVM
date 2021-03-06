#ifndef TYPES_H
#define TYPES_H
#include <systemc>
#define W   16
#define Q   SC_RND
#define O   SC_SAT_SYM
using namespace sc_dt;

typedef sc_fixed_fast <W,2,Q,O> din_t;
typedef sc_fixed_fast <W,2,Q,O> bin_t;
typedef sc_fixed_fast <W,2,Q,O> lin_t;
typedef sc_fixed_fast <W,6,Q,O> res_t;
typedef sc_fixed_fast <28,14,Q,O> acc_t;
typedef sc_fixed_fast <28,14,Q,O> p_t;
typedef sc_uint <4> num_t;

#endif
