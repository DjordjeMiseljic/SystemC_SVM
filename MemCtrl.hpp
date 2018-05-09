#ifndef MEMCTRL_H
#define MEMCTRL_H
#include <iostream>
#include <systemc>
#include <string>
#include <fstream>
#include <deque>
#include <vector>
#include <array>
#include <algorithm>
#include "Types.hpp"
using namespace std;
using namespace sc_core;

SC_MODULE(MemCtrl)
{
 public:
   SC_HAS_PROCESS(MemCtrl);
   MemCtrl(sc_module_name name, int& sv_num, int sv_len,
           sc_event* e_ready, sc_event* e_next, lin_t& lambda, 
           bin_t& bias, deque<din_t>& data);
   int num_of_lines(string str);
   int num_of_test_img();
 protected:
   int &sv_num;
   int sv_len; 
   sc_event *e_ready;
   sc_event *e_next;
   lin_t &lambda;
   bin_t &bias;
   deque <din_t> &data;
   
   array<deque<din_t>, 10> sv;
   //array<deque<double>, 10> targets;
   array<deque<lin_t>, 10> lambdas;
   deque<din_t> images;
   array<bin_t, 10> biases;
   array<int, 10> sv_lines;


   //METHODS
   void grab_from_mem();
   void file_extract();
   

};

#endif
