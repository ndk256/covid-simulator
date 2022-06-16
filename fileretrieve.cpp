#include "header.h"
#include <iostream>
#include <fstream> //neccesary?
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
  string fname; string selection; ifstream file;

Map map;

// Create AJAX objects to receive information from web page.
form_iterator type = cgi.getElement("ftype");
form_iterator filename = cgi.getElement("fname");
fname=**filename;

cout << "Content-Type: text/plain\n\n";

selection = **type;

if(selection=="g"||selection=="G"){
  file.open("/usr/local/var/www/txt-bin/"+fname); //replace this with your file path for text files
  if(!file.fail())
  {
    map.grid_init(file);
    file.close();
  }
  else{
    string sample_map = "                                                                                                    ";
    map.grid_init(10,10,sample_map);
    //current default is a blank 10x10 grid
  }

  map.print_state();
}
else if(selection=="p"||selection=="P"){
  file.open("/usr/local/var/www/txt-bin/"+fname); //replace this with your file path for text files
  if(!file.fail())
  {
    map.ppl_init(file);
    file.close();
  }
  else{
    string sample_register = "I 0 0 1 1 none\nS 1 1 1 0 surgical\nI 3 4 -1 0 cloth\nS 4 5 0 1 cloth\nS 7 7 -1 -1 KN95";
    map.ppl_init(sample_register);
  }
  map.print_ppl();
}
else{
  //cout<<"Unrecognized request";
  return 1;
}

return 0;

}
