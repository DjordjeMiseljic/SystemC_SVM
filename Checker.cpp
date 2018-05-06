#include "Checker.hpp"

Checker::Checker(sc_module_name name,  sc_event *e_fin, 
                 double &res, int &number): e_fin(e_fin),
                                            res(res),
                                            number(number)
{
   cout<<name<<" constucted"<<endl;
   SC_THREAD(verify);
}

void Checker::verify()
{
   int num=0;
   int true_number;
   int num_of_cl=0;
   int correct_cl=0;
   double percentage;
   int num_of_images=num_of_lines("../ML_number_recognition_SVM/saved_data/labels/labels.txt");
   double true_res;
   
   string r_line;
   string l_line;
   ifstream r_file("../ML_number_recognition_SVM/saved_data/results/res.txt");
   ifstream l_file("../ML_number_recognition_SVM/saved_data/labels/labels.txt");
   if(r_file.is_open() && l_file.is_open())
      while(num_of_images)
      {
         wait(*e_fin);

         if(num == 9)
            getline(r_file, r_line, '\n');
         else
            getline(r_file, r_line, ' ');
         true_res = stod(r_line);

         
         if(num == 9)
         {
            wait(1,SC_NS);
            getline(l_file, l_line);
            true_number = stoi(l_line);
            if(true_number==number)
               correct_cl++;
            else
            {
               cout<<"classified number: "<<number<<"["<<true_number<<"] :true_number";
               cout<<"         @"<<sc_time_stamp()<<"   #"<<name()<<endl;
            }
            
            num_of_images--;
            num_of_cl++; 
         }

         if(num<9)
            num++;
         else
            num=0;
         if(abs(true_res-res)>0.00001)
         {
            cout<<"ERROR: NUMBERS DON'T MATCH"<<endl;
            printf("core_res: %4f [%4f] :true_res",res,true_res);
            //cout<<"core_res: "<<res<<" ["<<true_res<<"] :true_res"; 
            cout<<"         @"<<sc_time_stamp()<<"   #"<<name()<<endl;
         }   
      }
   else
      cout<<"ERROR OPENING FILE"<<endl;

   percentage=(double)correct_cl/(double)num_of_cl;
   cout<<"number of classifications : "<<num_of_cl<<"\tpercentage: "<<100*percentage<<"%\t@"<<sc_time_stamp()<<"\t#"<<name()<<endl;
   r_file.close();
   l_file.close();
   return;	
}

int Checker::num_of_lines(string str)
{
   int count = 0;
   string line;
   ifstream sv_file(str);
   if(sv_file.is_open())

   {
      while(getline(sv_file,line))
         count++;
      sv_file.close();
   }
   else
      cout<<"error opening support vector file"<<endl;
   return count;
}

