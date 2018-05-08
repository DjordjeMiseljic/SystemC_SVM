#include <sc_dt>
#define W   16
#define Q   SC_RND
#define O   SC_SAT_SYM

typedef sc_fixed_fast <W,2,Q,O> din_t;
typedef sc_fixed_fast <W,4,Q,O> res_t;
typedef sc_fixed_fast <4,4,Q,O> num_t;
