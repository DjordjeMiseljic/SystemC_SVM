#ifndef TYPES_H
#define TYPES_H
#include <systemc>
#include <array>
#define SV_LEN 784
#define QUANTUM

#define W   12
#define Q   SC_RND
#define O   SC_SAT_SYM

using namespace sc_dt;
using namespace tlm;
using namespace std;
//sc_dt
const array<int, 10> sv_array = {466, 408, 827, 948, 683, 756, 533, 659, 1078, 924};
typedef sc_ufixed_fast <W,0,Q,O> din_t;
typedef sc_fixed_fast <W,1,Q,O> bin_t;
typedef sc_fixed_fast <W,0,Q,O> lin_t;
typedef sc_fixed_fast <W,6,Q,O> res_t;
typedef sc_fixed_fast <W+8,8,Q,O> acc_t;
typedef sc_fixed_fast <(12+W),14,Q,O> p_t;
typedef sc_uint <4> num_t;
//TLM
typedef tlm_base_protocol_types::tlm_payload_type pl_t;
typedef tlm_base_protocol_types::tlm_phase_type phase_t;

#endif
