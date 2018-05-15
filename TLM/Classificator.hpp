#ifndef CLASSIFICATOR_H
#define CLASSIFICATOR_H
#include <systemc>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <array>
#include <tlm>
#include "Types.hpp"
#include "Format.hpp"
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>

using namespace std;
using namespace sc_core;
using namespace tlm;

class Classificator :
   public sc_module
{
public:
   SC_HAS_PROCESS(Classificator);
   Classificator(sc_module_name name);
   
   tlm_utils::simple_target_socket<Classificator> s_cl_t;
   tlm_utils::simple_initiator_socket<Classificator> s_cl_i;
   
protected:
   void b_transport(pl_t&, sc_time&);
   //CLASSIFICATOR VARIABLES
   vector <din_t> image_v;
   vector <res_t> res_v;
   acc_t acc;
   p_t p;
   num_t cl_num;
   res_t max_res;

};

#endif
