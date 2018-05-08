#include <systemc>
#define W   16
#define Q   SC_RND
#define O   SC_SAT_SYM
using namespace sc_dt;

typedef sc_fixed_fast <W,2,Q,O> din_t;
typedef sc_fixed_fast <W,4,Q,O> acc_t;
typedef sc_fixed_fast <(2*W),10,Q,O> p_t;
typedef sc_ufixed_fast <4,4,Q,O> num_t;
