#include <LedControl.h>

#include "mpu6050.hpp" 
#include "Geom.hpp"

using namespace ldm;

#define DELAYTIME 1
#define TILTSENSORPIN 0
#define RANDOMSEEDPIN 1
#define LEDINTENSITY 4   // increase-decrease this, 8 is medium
#define LOWERINTENSITYLIMIT 50 // try 200 as a good number, higher the number slower to sleep 
#define SLEEPDELAY 2000

//LedControl library needs the following PINs/connections
#define DATAINPIN 12
#define CLOCKPIN 11
#define CHIPSELECTPIN 10

// last # says 3 matrix displays will be cascaded
LedControl lc=LedControl(DATAINPIN,CLOCKPIN, CHIPSELECTPIN, MATRIX_COUNT);

// initialize program data
Grav gGrav; //initialize garvity
Locations gLoc(gGrav); //Initialize the main Ball holder Locations object

void setup(){
  Serial.begin(115200);
  Serial.println("Starting");
  init_mpu();
  
  randomSeed(analogRead(RANDOMSEEDPIN)); 
  Serial.print(MATRIX_COUNT);
  // init LED matrices
  for(int address=0; address< MATRIX_COUNT; address++) {
    /*The MAX72XX is in power-saving mode on startup*/
    lc.shutdown(address,false);
    /* Set the brightness to a medium values */
    lc.setIntensity(address,LEDINTENSITY);
    /* and clear the display */
    lc.clearDisplay(address);
  }
  Serial.println(" Matrices are initialized");

  //initial readings are discarded until 
  int reading = 300;  
  while(reading--){
    read_mpu();
    int newreading = ypr[1];
  }
}


bool isSleeping = false;
int  previous = 0;
int  slpcounter = 0;
int  intensity = LEDINTENSITY;

void resetIntensity(){
  intensity = LEDINTENSITY;
  for(int address=0; address< MATRIX_COUNT; address++) {
    lc.shutdown(address,false);
    lc.setIntensity(address, intensity);
  }
}


bool shouldWakeUp(){
  int checkread = readGravityInput();
  int c=0;
  while (c++< 30) // disread these
    checkread = readGravityInput();
  if (checkread == previous){
    //Serial.print(checkread);
    //Serial.println(" Reading is same");    
    return false;
  }else{
    //Serial.println("Should wake up!");    
    previous = checkread;
    return true;
  }  
}

void loop(){
  if (isSleeping){
    if (!shouldWakeUp()){
      delay(SLEEPDELAY);
      return;
    }else{
      resetIntensity();
      intensity = LEDINTENSITY;
      slpcounter = 0;
      isSleeping = false;
    }
  }
  int reading = readGravityInput();
  if (reading == previous)
    slpcounter++;
  else{
    slpcounter = 0;
    if (intensity != LEDINTENSITY)
      resetIntensity();
  }
  previous = reading;
  gGrav.updateGravity(reading);
  gLoc.applyGravity(gGrav.g);
  displayLocations();
  delay(DELAYTIME);
  if (slpcounter > LOWERINTENSITYLIMIT){
    slpcounter = 0;
    if(intensity != 0)
      intensity--;
    else{
      for(int i=0; i<MATRIX_COUNT; i++)
	lc.shutdown(i, true);
      isSleeping =true;
      return;
    }
    for(int i=0; i<MATRIX_COUNT; i++)
      lc.setIntensity(i, intensity);
  }
    
}


inline void displayLocations(){
  for (int r=0; r<=MAX_ROW; r++){
     int mat = r / MATRIX_SIZE;
     // This following line is due to the orientation of
     // the mounting of the  matrix displays on the project housing 
     // if I mounted them 180 degrees rotated clockwise
     // it would not have been needed
     int row = MAX_COL - (r % MATRIX_SIZE); 
     lc.setRow(mat, row, gLoc.viewData(r));
  }
}

int readGravityInput(){
  read_mpu();
  int i= (int) (ypr[1] * 180/M_PI); //return PITCH
  //Serial.println(ypr[1]);
  return i;
}


