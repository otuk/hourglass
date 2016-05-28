#ifdef PCRUN

#include <iostream>
#include <cassert>
#include <unistd.h> //usleep
#include <ctime>
#include <cstdlib>
#include "macros.h"
#include "Geom.hpp"

#define TEST_CASE(testcall) \
  PRINTERR("TC: " << testcall <<" executed");

using namespace ldm;
using namespace std;

float ypr[] ={0, 0, 0};

static int testTotal = 0;
int test_grav();
void display();


int main(int argc, char *argv[])
{
  if (argc > 1){
    display();
    return 0;
  }   
  TEST_CASE(test_grav());
  PRINTERR("All test executed succesfully");
  PRINTERR(" If you want to execute simulation display run './test_hg display'");
  return 0;
}


int test_grav(){
  Grav grav;
  assert(grav.countl() ==0);
  grav.updateGravity(10);
  assert(grav.g[FALL][X] == 0);
  assert(grav.g[LEFT][X] == -1);
  assert(grav.g[LEFT][Y] == 1);
  return ++testTotal;
}


//PC Simulation of the LED matrix
void display(){
  srand(time(NULL));
  Grav grav;
  Locations loc(grav);
  PRINTERR("Locations created");
  loc.display();
  grav.updateGravity(10);
  const int REPEAT = 400;
  const long SLEEP = 10000;
  const int TIMES = 16;
  for (int t=0; t<TIMES; t++){
    //test only line
    grav.g = grav.gfrl[t%8];
    // in arduino the sensor decides the gravity array
    for (int i=0; i<REPEAT; i++){
	loc.applyGravity(grav.g);
	loc.display();
	usleep(SLEEP);
    }
  }
}

#endif
