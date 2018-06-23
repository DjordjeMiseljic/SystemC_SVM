#ifndef DESKEW_H
#define DESKEW_H
#include <systemc>
#include <tlm>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>
#include "Format.hpp"
#include "Types.hpp"
#include <string>
#include <array>
#include <vector>
using namespace std;
using namespace sc_core;
using namespace tlm;

class Deskew : public sc_module
{
   public:
      Deskew(sc_module_name name);
      //tlm_utils::simple_initiator_socket<Deskew> s_de_i;
      tlm_utils::simple_target_socket<Deskew> s_de_t;
   protected:
      void b_transport(pl_t&, sc_time&);
      void calc_moments(vector<din_t> image, p_t& mu11, p_t& mu02);
      vector<din_t> deskew(vector <din_t> image);
      vector<din_t> image;
};
#endif
