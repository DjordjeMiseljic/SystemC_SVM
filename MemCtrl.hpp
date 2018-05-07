#ifndef MEMCTRL_H
#define MEMCTRL_H
#include <iostream>
#include <systemc>
#include <string>
#include <fstream>
#include <deque>
#include <vector>
#include <array>
using namespace std;
using namespace sc_core;

SC_MODULE(MemCtrl)
{
 public:
   SC_HAS_PROCESS(MemCtrl);
   MemCtrl(sc_module_name name, int& sv_num, int sv_len,
           sc_event* e_ready, sc_event* e_next, double& lambda, 
           int& target, deque<double>& data, double& res);
   int num_of_lines(string str);
   int num_of_test_img();
 protected:
   int &sv_num;
   int sv_len; 
   sc_event *e_ready;
   sc_event *e_next;
   double &lambda;
   int &target;
   deque <double> &data;
   double &res;
   
   array<deque<double>, 10> sv;
   array<deque<double>, 10> targets;
   array<deque<double>, 10> lambdas;
   deque<double> images;
   array<double, 10> biases;
   array<double, 10> sv_lines;


   //METHODS
   void grab_from_mem();
   void file_extract();
   

};

#endif
