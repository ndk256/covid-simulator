#ifndef CLASS_H
#define CLASS_H

#include<vector>
#include<string>
#include<cmath> //for pow()
#include<fstream>

const int ZZZ=30; //Can definitely increase this further
const int RES_PERIOD=3*30*24*60; //value is based on an approximately 3-month resistance period
const double P_INF=0.324; //value is from a spring 2020 study conducted on a university campus
const double INF_RAD=2; //infection radius, in meters (note: somewhat filler value)

using namespace std;

class Cell{
public:
  char contents;
  int ref; //to link to Person/other array

  Cell(); //ctor
  bool open(); //whether Cell has contents
  bool init(char content, int ind);
  void receive(Cell &from);
};
  void clear(Cell &c);

class Person{
public:
  char inf_status; // i/s/r
  int state_time; // time when inf_status was acquired
  int posn_x, posn_y;
  int vec_x, vec_y; // intended direction
  int head_x, head_y; // for facing
  bool vaccinated;
  char mask_type; // none (' ' or 'u'), cloth ('c'), surgical ('s'), KN95/FFP2 ('q')

  Person(char status, bool vacc, int px, int py, int ts, int vx, int vy, string mask); //ctor
  Person(string data); //constructs Person from a line of string
  void time_updates(int ts);
  int getXpos(); int getYpos(); //accessors
  double rec_prob(int ts);
};

double inf_prob(Person infector, Person susceptible);

class Map{ //singleton class
public:
  Cell grid[ZZZ][ZZZ]; // grid (has maximum size of ZZZ*ZZZ)
  int gx, gy; // dimensions (x and y, respectively) of the grid
  vector<Person> people;
  vector<int> numbers; // vector used for random ordering in updates
  vector<int> i_people; // list of infected people (indices)
  int casecount;

  Map(); //ctor
  void ppl_init(ifstream &pplfile); //initialize people array from a file
  void ppl_init(string temp_pplinit); //^ but from a string (with newlines)
  void grid_init(int gridx, int gridy, string temp_mapinit); //initialize grid
  void grid_init(ifstream &gridfile); //initialize grid from file
  void update(int ts);
  void infection_update(int ts);
  void print_state(); //TODO: param for ostream?
  void print_ppl();
  void move(Person &person);
  vector<int> check_cone(Person p, double r);
  //vector<int> check_radius(int x, int y, double r);
};

#endif
