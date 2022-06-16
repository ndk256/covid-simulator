//map.cpp
#ifndef MAP
#define MAP

#include "header.h"
#include <iostream>
#include <cstring>
#include <fstream>
#include <algorithm> //for shuffle()
//#include <random> //for shuffle? //auto rng = std::default_random_engine {};
//auto rd = std::random_device {};
//auto rng = std::default_random_engine { rd() };

//constructor for UNINITIALIZED map
Map::Map(){
  gx=ZZZ; gy=ZZZ;
}

//This function takes a grid and a Person that is on a position in that grid
//and it moves the person one cell in a direction within +/- 90 degrees of their vector
void Map::move(Person &person){
  //ensure the person's vector will not take them off the grid
  if(person.getXpos() + person.vec_x >= gx) person.vec_x = 0;
  if(person.getXpos() + person.vec_x < 0) person.vec_x = 0;
  //one way: go perpendicular
  int destx = person.getXpos() + person.vec_x;

  //another way: reverse direction
  if(person.getYpos() + person.vec_y >= gy) person.vec_y *= -1;
  if(person.getYpos() + person.vec_y < 0) person.vec_y *= -1;
  int desty = person.getYpos() + person.vec_y;

  //re-pathing (if the destination space is occupied)
  if(!grid[destx][desty].open()){
    if(destx==person.getXpos()){ //if purely vertical movement
      //try the diagonals
        if(destx-1 >=0 && grid[destx-1][desty].open()) {destx--;
        }
        else if(destx+1 < gx && grid[destx+1][desty].open()) {destx++;
        }
        else
          {
            desty = person.getYpos(); //try horizontal movement
            if(destx+1 < gx && grid[destx+1][desty].open()) destx--;
            else if(destx-1 >= 0 && grid[destx-1][desty].open()) destx++;
            //and if none of those work, just give up
            else{
              destx = person.getXpos(); desty=person.getYpos();
            }
          }
      }
    else if(desty==person.getYpos()){ //if purely horizontal movement
      if(desty-1 >= 0 && grid[destx][desty-1].open()) desty--;
      else if(desty+1 < gy && grid[destx][desty+1].open()) desty++;
      else
        {
          destx = person.getXpos(); //try vertical movement
          if(destx+1 < gx && grid[destx+1][desty].open()) destx--;
          else if(destx-1 >= 0 && grid[destx-1][desty].open()) destx++;
        //and if none of those work, just give up
        else{
          destx = person.getXpos(); desty=person.getYpos();
        }
      }
    }
      else //if diagonal movement (or set to no movement at all)
      {
         //try vertical movement
         if(grid[person.getXpos()][desty].open()) destx=person.getXpos();
         //try horizontal movement
         else if(grid[destx][person.getYpos()].open()) desty=person.getYpos();
         //try the perpendicular diagonals
         else {
           if(grid[destx-2*person.vec_x][desty].open()) {destx-=2*person.vec_x;
           }
           else if(grid[destx][desty-2*person.vec_y].open()) desty-=2*person.vec_y;
           //and if that doesn't work, give up
           else{
             destx = person.getXpos(); desty=person.getYpos();
           }
        }
      }
    } //end dealing with re-pathing

  //compute facing
  person.head_x = destx - person.getXpos();
  person.head_y = desty - person.getYpos();

  //if the person ended up without a vector,
  // give them one based on where they're going now
  if(person.vec_x == 0 && person.vec_y == 0)
    {
      person.vec_x = person.head_x;
      person.vec_y = person.head_y;
    }

  //actually move the person
  grid[destx][desty].receive(grid[person.getXpos()][person.getYpos()]);
  person.posn_x = destx; person.posn_y = desty;
}

//This function updates the infection status of people in the map
// based on their spatial relationship to infected people
// and the comparison of a randomly generated number to the infection probability
void Map::infection_update(int ts){
  for(int j=0; j<i_people.size(); j++){
    if(people[i_people[j]].inf_status=='i'&&(ts-people[i_people[j]].state_time)>0)
    {
      vector<int> susc = check_cone(people[i_people[j]], INF_RAD);
      for(int i=0; i<susc.size(); i++){
        if(people[susc[i]].inf_status=='s'){
          if(rand()/RAND_MAX<inf_prob(people[i_people[j]],people[susc[i]])) //infection roll
          {
            people[susc[i]].inf_status='i';
            people[susc[i]].state_time= ts;
            i_people.push_back(susc[i]);
            casecount++;
          } //end if for infection roll
        } //end if(susceptible)
      } //end iteration of susc
    } //end if(infected)
  }
return;
}

//This function moves people on the map, computes whether or not they become infected
// and whether or not they become recovered in the time step ts
void Map::update(int ts){
  //movement of people
  random_shuffle(numbers.begin(),numbers.end());
  for(int i=0; i<people.size(); i++){
    int num = numbers[i];
    move(people[num]);
  }

  //infection of people
  random_shuffle(numbers.begin(),numbers.end());
  infection_update(ts);

  //recovery/resistance status of people
  for(int i=0; i<people.size(); i++){
    people[i].time_updates(ts);
  }

  return;
}

//This function checks a triangular area in front of a Person
// from the rth diagonals to the rth vertical/horizontal, such that
// the far boundary of the area is a straight line.
// It does not check spaces which are beyond an occupied grid square in the same direction.
// It then returns the indexes of Person objects within that area
// who are susceptible in a vector.
vector<int> Map::check_cone(Person p, double r){
  vector<int> susc;
  int radius=floor(r);

  if(abs(p.head_x)==abs(p.head_y)) //diagonal facing
  {
    //the diagonal itself
    for(int j=1; j<=radius; j++){
      if(grid[p.getXpos()+j*p.head_x][p.getYpos()+j*p.head_y].contents!=' '||(p.getXpos()+j*p.head_x)<=0||(p.getXpos()+j*p.head_x)>=gx-1||
        (p.getYpos()+j*p.head_y)<=0||(p.getYpos()+j*p.head_y)>=gy-1){
        j=radius; //end loop early
      }
      if(grid[p.getXpos()+j*p.head_x][p.getYpos()+j*p.head_y].contents=='P'
        && people[grid[p.getXpos()+j*p.head_x][p.getYpos()+j*p.head_y].ref].inf_status=='s')
        susc.push_back(grid[p.getXpos()+j*p.head_x][p.getYpos()+j*p.head_y].ref);
    }
    //the 'section' closer to the x-axis
    for(int j=0;j<radius;j++){
      for(int i=j+1; i<=radius; i++){
        if(grid[p.getXpos()+i*p.head_x][p.getYpos()+j*p.head_y].contents=='P'
          && people[grid[p.getXpos()+i*p.head_x][p.getYpos()+j*p.head_y].ref].inf_status=='s')
            susc.push_back(grid[p.getXpos()+i*p.head_x][p.getYpos()+j*p.head_y].ref);
        if(grid[p.getXpos()+i*p.head_x][p.getYpos()+j*p.head_y].contents!=' '||(p.getXpos()+i*p.head_x)<=0||(p.getXpos()+i*p.head_x)>=gx-1||
          (p.getYpos()+j*p.head_y)<=0||(p.getYpos()+j*p.head_y)>=gy-1){
          i=radius+1; //end loop early (todo:check this)
        }
      }
    }
    //section closer to the y-axis
    for(int j=0;j<radius;j++){
      for(int i=j+1; i<=radius; i++){
        if(grid[p.getXpos()+j*p.head_x][p.getYpos()+i*p.head_y].contents=='P'
          && people[grid[p.getXpos()+j*p.head_x][p.getYpos()+i*p.head_y].ref].inf_status=='s')
            susc.push_back(grid[p.getXpos()+j*p.head_x][p.getYpos()+i*p.head_y].ref);
        if(grid[p.getXpos()+j*p.head_x][p.getYpos()+i*p.head_y].contents!=' '||(p.getXpos()+j*p.head_x)<=0||(p.getXpos()+j*p.head_x)>=gx-1||
          (p.getYpos()+i*p.head_y)<=0||(p.getYpos()+i*p.head_y)>=gy-1){
          i=radius+1; //end early (todo: check)
        }
      }
    }
  }//end diagonal case
  else if(p.head_x==0) //vertical facing
  {
    //"narrow" cone (pos)
    for(int j=0;j<radius;j++){
      for(int i=j+1; i<=radius; i++){
        if(grid[p.getXpos()+j][p.getYpos()+i*p.head_y].contents=='P'
          && people[grid[p.getXpos()+j][p.getYpos()+i*p.head_y].ref].inf_status=='s')
            susc.push_back(grid[p.getXpos()+j][p.getYpos()+i*p.head_y].ref);
        if(grid[p.getXpos()+j][p.getYpos()+i*p.head_y].contents!=' '||(p.getXpos()+j)<=0||(p.getXpos()+j)>=gx-1||
          (p.getYpos()+i*p.head_y)<=0||(p.getYpos()+i*p.head_y)>=gy-1){
          i=radius+1; //end early (todo: check)
        }
      }
    }
    //narrow cone (neg)
    for(int j=0;j<radius;j++){
      for(int i=j+1; i<=radius; i++){
        if(grid[p.getXpos()-j][p.getYpos()+i*p.head_y].contents=='P'
          && people[grid[p.getXpos()-j][p.getYpos()+i*p.head_y].ref].inf_status=='s')
            susc.push_back(grid[p.getXpos()-j][p.getYpos()+i*p.head_y].ref);
        if(grid[p.getXpos()-j][p.getYpos()+i*p.head_y].contents!=' '||(p.getXpos()-j)<=0||(p.getXpos()-j)>=gx-1||
          (p.getYpos()+i*p.head_y)<=0||(p.getYpos()+i*p.head_y)>=gy-1){
          i=radius+1; //end early (todo: check)
        }
      }
    }
    //left diagonal
    for(int j=1; j<=radius; j++){
      if(grid[p.getXpos()-j][p.getYpos()+j*p.head_y].contents=='P'
        && people[grid[p.getXpos()-j][p.getYpos()+j*p.head_y].ref].inf_status=='s')
        susc.push_back(grid[p.getXpos()-j][p.getYpos()+j*p.head_y].ref);
      if(grid[p.getXpos()-j][p.getYpos()+j*p.head_y].contents!=' '||(p.getXpos()-j)<=0||(p.getXpos()-j)>=gx-1||
        (p.getYpos()+j*p.head_y)<=0||(p.getYpos()+j*p.head_y)>=gy-1){
        j=radius; //end loop early (todo:check for off by one)
      }
    }
    //right diagonal
    for(int j=1; j<=radius; j++){
      if(grid[p.getXpos()+j][p.getYpos()+j*p.head_y].contents=='P'
        && people[grid[p.getXpos()+j][p.getYpos()+j*p.head_y].ref].inf_status=='s')
        susc.push_back(grid[p.getXpos()+j][p.getYpos()+j*p.head_y].ref);
      if(grid[p.getXpos()+j][p.getYpos()+j*p.head_y].contents!=' '||(p.getXpos()+j)<=0||(p.getXpos()+j)>=gx-1||
        (p.getYpos()+j*p.head_y)<=0||(p.getYpos()+j*p.head_y)>=gy-1){
        j=radius; //end loop early (todo:check for off by one)
      }
    }
    //vertical line
    for(int j=1;j<=radius;j++){
      if(grid[p.getXpos()][p.getYpos()+j*p.head_y].contents=='P'
        && people[grid[p.getXpos()][p.getYpos()+j*p.head_y].ref].inf_status=='s')
        susc.push_back(grid[p.getXpos()][p.getYpos()+j*p.head_y].ref);
      if(grid[p.getXpos()][p.getYpos()+j*p.head_y].contents!=' '||(p.getXpos())<=0||(p.getXpos())>=gx-1||
        (p.getYpos()+j*p.head_y)<=0||(p.getYpos()+j*p.head_y)>=gy-1){
        j=radius; //end loop early (todo:check for off by one)
      }
    }
  }//end else if (vertical)
  else //horizontal facing
  {
    //"narrow" cone (pos)
    for(int j=0;j<radius;j++){
      for(int i=j+1; i<=radius; i++){
        if(grid[p.getXpos()+i*p.head_x][p.getYpos()+j].contents=='P'
          && people[grid[p.getXpos()+i*p.head_x][p.getYpos()+j].ref].inf_status=='s')
            susc.push_back(grid[p.getXpos()+i*p.head_x][p.getYpos()+j].ref);
        if(grid[p.getXpos()+i*p.head_x][p.getYpos()+j].contents!=' '||(p.getXpos()+i*p.head_x)<=0||(p.getXpos()+i*p.head_x)>=gx-1||
          (p.getYpos()+j)<=0||(p.getYpos()+j)>=gy-1){
          i=radius+1; //end early (todo: check)
        }
      }
    }
    //narrow cone (neg)
    for(int j=0;j<radius;j++){
      for(int i=j+1; i<=radius; i++){
        if(grid[p.getXpos()+i*p.head_x][p.getYpos()-j].contents=='P'
          && people[grid[p.getXpos()+i*p.head_x][p.getYpos()-j].ref].inf_status=='s')
            susc.push_back(grid[p.getXpos()+i*p.head_x][p.getYpos()-j].ref);
        if(grid[p.getXpos()+i*p.head_x][p.getYpos()-j].contents!=' '||(p.getXpos()+i*p.head_x)<=0||(p.getXpos()+i*p.head_x)>=gx-1||
          (p.getYpos()-j)<=0||(p.getYpos()-j)>=gy-1){
          i=radius+1; //end early (todo: check)
        }
      }
    }
    //lower diagonal
    for(int j=1; j<=radius; j++){
      if(grid[p.getXpos()-j*p.head_x][p.getYpos()+j].contents=='P'
        && people[grid[p.getXpos()-j*p.head_x][p.getYpos()+j].ref].inf_status=='s')
        susc.push_back(grid[p.getXpos()-j*p.head_x][p.getYpos()+j].ref);
      if(grid[p.getXpos()-j*p.head_x][p.getYpos()+j].contents!=' '||(p.getXpos()-j*p.head_x)<=0||(p.getXpos()-j*p.head_x)>=gx-1||
        (p.getYpos()+j)<=0||(p.getYpos()+j)>=gy-1){
        j=radius; //end loop early (todo:check for off by one)
      }
    }
    //upper diagonal
    for(int j=1; j<=radius; j++){
      if(grid[p.getXpos()+j*p.head_x][p.getYpos()+j].contents=='P'
        && people[grid[p.getXpos()+j*p.head_x][p.getYpos()+j].ref].inf_status=='s')
        susc.push_back(grid[p.getXpos()+j*p.head_x][p.getYpos()+j].ref);
      if(grid[p.getXpos()+j*p.head_x][p.getYpos()+j].contents!=' '||(p.getXpos()+j*p.head_x)<=0||(p.getXpos()+j*p.head_x)>=gx-1||
        (p.getYpos()+j)<=0||(p.getYpos()+j)>=gy-1){
        j=radius; //end loop early todo:(check for off by one)
      }
    }
    //horizontal line
    for(int j=1;j<=radius;j++){
      if(grid[p.getXpos()+j*p.head_x][p.getYpos()].contents=='P'
        && people[grid[p.getXpos()+j*p.head_x][p.getYpos()].ref].inf_status=='s')
        susc.push_back(grid[p.getXpos()+j*p.head_x][p.getYpos()].ref);
      if(grid[p.getXpos()+j*p.head_x][p.getYpos()].contents!=' '||(p.getXpos()+j*p.head_x)<=0||(p.getXpos()+j*p.head_x)>=gx-1||
        (p.getYpos())<=0||(p.getYpos())>=gy-1){
        j=radius; //end loop early (todo:check for off by one)
      }
    }
  }

  return susc;
}

//this function prints the map's current (populated) grid to std::cout
void Map::print_state(){
for(int i=0; i<gy; i++){
  for(int j=0; j<gx; j++){
    if(grid[j][i].contents=='P'){
     if(people[grid[j][i].ref].inf_status=='i')  cout << "I";
     else if(people[grid[j][i].ref].inf_status=='r') cout << "R";
     else cout << "S";
      }
    else if(grid[j][i].contents==' ')
      cout << " ";
    else cout << "X";
    }
    cout << endl;
  }
}

//this function prints the list of people with their information to std::cout
void Map::print_ppl(){
  //cout<<"=" << people.size();//debug 
  for(int i=0; i<people.size(); i++){
    cout << static_cast<char>(toupper(people[i].inf_status)) << " ";
    if(people[i].vaccinated) cout << "v ";
    else cout << "u ";
    cout << people[i].getXpos() << " " << people[i].getYpos() << " ";
    cout << people[i].head_x << " " << people[i].head_y << " "
      << people[i].mask_type
      << "/"; //for frontend
  }
}

//this function initializes the grid (barriers/walkable spaces) from an input file
// which must be formatted with (preferably) equal lines each ended by \n
//note that the x length is taken from the length of the first line
void Map::grid_init(ifstream &gridfile){
    string temp;
    gy = 0;

    getline(gridfile, temp, '\n');
    gx = temp.length(); //base the x length on the length of the first line

    while(!gridfile.fail() && gy<ZZZ){
      for(int i=0; i<ZZZ; i++)
      {
        if(i<temp.length()&&i<gx)
        {
          grid[i][gy].init(temp[i],0);
        }
        else
        {
          grid[i][gy].init('X',0);
        }
      }
      gy++;
      getline(gridfile, temp, '\n');
    }
    for(int i=gy; i<ZZZ; i++){
      for(int j=0;j<ZZZ;j++){
      grid[j][i].init('X',0);
      }
    }

}

//This function takes the two integer dimensions of the grid
// and a string for the grid details with NO line separators,
// and initializes the grid member of the Map class
void Map::grid_init(int gridx, int gridy, string temp_mapinit){
  char cgrid[gridx*gridy+1];
  strcpy(cgrid,temp_mapinit.c_str());

  gx=gridx; gy=gridy;

  for(int i=0; i<gridx; i++){
    for(int j=0; j<gridy; j++){
      if(j*gridx+i >= temp_mapinit.length()) //if there is less input than expected
        grid[i][j].init(' ',0); //make the space empty
      else
        grid[i][j].init(cgrid[j*gridx+i], 0); //initialize with provided value
    }
  }
return;
}

//This function takes in a string
// listing the specifications for Person objects, separated by newlines,
// constructs the objects,
// and fills the people, numbers and i_people vectors appropriately
// It also initializes the casecount
void Map::ppl_init(string temp_pplinit){
  temp_pplinit = temp_pplinit+"\n";//duct-tape fix ig //TODO: is this still needed?
  replace(temp_pplinit.begin(),temp_pplinit.end(),'/','\n');//for frontend
  replace(temp_pplinit.begin(),temp_pplinit.end(),'_',' ');//for frontend
    
  int mk=0, ind=temp_pplinit.find('\n',mk); //indexing variables to help move through the string
  string temp;

  do{
    temp = temp_pplinit.substr(mk, ind-mk);
    Person ptemp(temp);
    if(grid[ptemp.getXpos()][ptemp.getYpos()].init('P', people.size())){
     //only put the person into the list if they can be placed
        people.push_back(ptemp);
        numbers.push_back(people.size()-1);
        if(ptemp.inf_status=='i')
            i_people.push_back(people.size()-1);
        }

    mk = ind+1;
    ind = temp_pplinit.find('\n',mk);
  }while(ind<=temp_pplinit.length()-1);

  casecount = i_people.size();

  return;
}

//This function initalizes the people, i_people, numbers and casecount members
// from a file, which should have each Person's information separated by newlines
void Map::ppl_init(ifstream &pplfile){
  string temp;

  while(!pplfile.fail()){
    getline(pplfile, temp, '\n');
    Person ptemp(temp);
    if(grid[ptemp.getXpos()][ptemp.getYpos()].init('P', people.size())){
        //only put the person into the list if they can be placed
        people.push_back(ptemp);
        numbers.push_back(people.size()-1);
        if(ptemp.inf_status=='i')
          {
            i_people.push_back(people.size()-1);}
          }
      }

    casecount = i_people.size();

    return;
}

#endif
