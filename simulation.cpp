#include "header.h"
#include <iostream>
#include <fstream> //neccessary?
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
  
  int gx = stoi(**x); int gy = stoi(**y);
  
  cout << "Content-Type: text/plain\n\n"; //start of response
  map.grid_init(gx,gy,gname);
  
if(pop_load_file){
  file2.open("/usr/local/var/www/txt-bin/"+pname);
  if(!file2.fail())
  {
    map.ppl_init(file2);
    file2.close();
  }
  else{
    //failed to load the file
    string sample_register = "I 2 3 1 1 none\nS 3 3 -1 1 surgical\nI 3 4 -1 0 cloth\nS 4 5 0 1 cloth\nS 7 7 -1 -1 KN95";
    map.ppl_init(sample_register);
  }
}
else{
  map.ppl_init(pname);
}
  
  int ts=0;
  if(stoi(**t)>0) ts = stoi(**t);

  map.print_state(); //prints starting map
  cout << "=";
  
  for(int t=0; t<ts; t++){
    map.update(t);
    map.print_state();
    cout << "=";
  }

  cout << map.casecount << endl;

return 0;

}
