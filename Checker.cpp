#include "Checker.hpp"
#include "Format.hpp"
Checker::Checker(sc_module_name name,  sc_event *e_fin, 
                 double &res, int &number, double &max_acc): e_fin(e_fin),
                                                             res(res),
                                                             number(number),
                                                             max_acc(max_acc)
{
   cout<<"Checker constucted"<<endl;
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
            {
               correct_cl++;
               cout<<B_GREEN<<"CORRECT CLASSIFICATION"<<GREEN<<" : classified number : "<<number<<"["<<true_number<<"] :true_number";
               cout<<RST<<DIM<<"         @"<<sc_time_stamp()<<"   #"<<name()<<RST<<endl;
            }
            else
            {
               cout<<B_RED<<"MISCLASSIFICATION"<<RED<<" : classified number : "<<number<<"["<<true_number<<"] :true_number";
               cout<<RST<<DIM<<"         @"<<sc_time_stamp()<<"   #"<<name()<<RST<<endl;
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
            cout<<BC_YELLOW<<"WARNING"<<BC_RST<<YELLOW<<" res mismatch   : ";
            printf("core_res: %4f [%4f] :true_res",res,true_res);
            //cout<<"core_res: "<<res<<" ["<<true_res<<"] :true_res"; 
            cout<<RST<<DIM<<"     @"<<sc_time_stamp()<<"   #"<<name()<<RST<<endl;
         }   
      }
   else
      cout<<BC_RED<<"ERROR"<<BC_RST<<RED<<" OPENING RES/LABEL FILE"<<RST<<endl;

   percentage=(double)correct_cl/(double)num_of_cl;
   cout<<"number of classifications : "<<num_of_cl<<GREEN<<"\tpercentage: "<<B_GREEN<<100*percentage<<"%\t@"<<RST<<DIM<<sc_time_stamp()<<"\t#"<<name()<<RST<<endl;
   cout<<"maximum accumulated number : "<<max_acc<<endl;
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
   {

      cout<<BC_RED<<"ERROR"<<BC_RST<<RED<<" OPENING SV FILE"<<endl;
      cout<<RST<<DIM<<"         @"<<sc_time_stamp()<<"   #"<<name()<<RST<<endl;
   }
   return count;
}

