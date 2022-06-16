#include "header.h"
#include <iostream>
#include <fstream>//necc?
// Stuff for AJAX
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"

#define XML_USE_STL
using namespace std;
using namespace cgicc; // Needed for AJAX functions

int main()
{
  // Object used for receiving AJAX call (and getting parameters)
  Cgicc cgi;    // Ajax object
  Map map;
  ifstream file2;

  //string sample_map = "                                                                                                    ";
//"XXX X   X      XX X X    XXX X     XX X X    XXX X     XX X X    XXX X      X X X     XX X X       X";

  // Create AJAX objects to receive information from web page.
  form_iterator x = cgi.getElement("xdim");
  form_iterator y = cgi.getElement("ydim");
  form_iterator grid = cgi.getElement("grid");
  form_iterator pop_fromfile = cgi.getElement("pf");
  form_iterator pop = cgi.getElement("pop");
  form_iterator t = cgi.getElement("timesteps");

  string gname = **grid;
  string pname = **pop;
  bool pop_load_file = (**pop_fromfile == "1");
  
//  sample_map=**grid;
  int gx = stoi(**x); int gy = stoi(**y);
  
  cout << "Content-Type: text/plain\n\n";
  //cout << "Received:\n\n";
  //cout << gname;
//  cout << "\nwhich is " << gname.length() << " vs " << gx*gy;

  /*file.open("/var/www/html/class/softdev/old/kelleher1/web/capstone/files/"+gname);
  if(!file.fail())
  {
    map.grid_init(file);
  }
  else{
    string sample_map = "                                                                                                    ";
    map.grid_init(10,10,sample_map);
    //current default is a blank 10x10 grid
  }
  file.close();
  */
//  cout << "init with " << gx << " x " << gy << " using: =";//debug
//  cout << gname << "=";//debug
  map.grid_init(gx,gy,gname);
  
//  sample_register = **pop;
if(pop_load_file){
  file2.open("/usr/local/var/www/txt-bin/"+pname);
  if(!file2.fail())
  {
    map.ppl_init(file2);
    file2.close();
    //cout << "printing people\n";//debug
    //map.print_ppl();//temp
  }
  else{
    //cout << "Content-Type: text/plain\n\n";
    //cout << "Failed to load file, using default population";
    string sample_register = "I 2 3 1 1 none\nS 3 3 -1 1 surgical\nI 3 4 -1 0 cloth\nS 4 5 0 1 cloth\nS 7 7 -1 -1 KN95";
    map.ppl_init(sample_register);
    //map.print_ppl();
  }
}
else{
  //cout << "received:"<<pname<<"=";
  map.ppl_init(pname);
}
  
  int ts=0;
  if(stoi(**t)>0) ts = stoi(**t);

  //cout << "Content-Type: text/plain\n\n";
  //cout<<"Starting map:\n";
  map.print_state(); //prints starting map
  cout << "=";
  
  for(int t=0; t<ts; t++){
    map.update(t);
    //map.print_ppl(); cout << "=";//DEBUG 
    map.print_state();
    cout << "=";
  }

  cout << map.casecount << endl;

return 0;

}