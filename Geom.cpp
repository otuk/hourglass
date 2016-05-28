#include "Geom.hpp"

#define MAXBOUND 1.10 // arbitrary # filters/controls sensor updates
#define MINBOUND 0.90 // arbitrary # filters/controls sensor updates

using namespace ldm;

#ifdef PCRUN
//ball shape - PC test only
const char* const ldm::SYM_BALL = "\u25CF";
//mask shape  - PC test only
const char* const ldm::SYM_MASK = "\u25A1";  
#endif

const unsigned char Locations::HGMASK[] = {
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,

  0b10000001,
  0b11000011,
  0b11000011,
  0b11100111,
  0b11100111,
  0b11000011,
  0b11000011,
  0b10000001,

  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000
    
};


//TODO -add being start increment finnert arrow? angle name
// gfrl is organized as follows
// gfrl[compas direction][Fall, RollRight, Rollleft][X, Y]
// gfrl[0][ROLLRIGHT][Y] shows the gravity in directionn
// y-axis when object top is pointing North ie sitting upright
const char Grav::gfrl[][3][2] = {
  { { 0, 1}, { 1, 1}, {-1, 1} },  // N
  
  { { 1, 1}, { 1, 0}, { 0, 1} },   // NW

  { { 1, 0}, { 1,-1}, { 1, 1} },  // W

  { { 1,-1}, {0, -1}, { 1, 0} },  // SW

  { { 0,-1}, {-1,-1}, { 1,-1} },  // S

  { {-1,-1}, {-1, 0}, { 0,-1} },  // SE

  { {-1, 0}, {-1, 1}, {-1,-1} },  // E

  { {-1, 1}, { 0, 1}, {-1, 0} }  // NE
				 
};

// this is the array the MPU6050 library exposes
// to read the rotation angles YAW-PITCH-ROLL
extern float ypr[3];

Grav::Grav(){
  g = &(gfrl[0][0]);  //arbitrarily set it to initial array value "N"
}

int Grav::countl() const{
  return mCountl;
}

void Grav::updateGravity(int m){
  // TBD change how G values impact ball acceleration
  // to avoid adding floats to Ball
  // maybe change FInert level for each gravity array
  if ( 115 >m && m >= 70)
    g = & gfrl[4][0];                 // S 
  else  if ( 70 > m && m >= 25  ) {
    if (ypr[2] * 180/M_PI >0)
      g = & gfrl[3][0];               // SW
    else  
      g = & gfrl[5][0];               // SE
   }else  if ( (25 > m && m >= 0 ) || (m>0 && m>=-20)  ){
    if (ypr[2] * 180/M_PI >0)
      g = & gfrl[2][0];               // W
    else  
      g = & gfrl[6][0];               // E
  }else  if ( -20 > m && m >= -65  ) { 
     if (ypr[2] * 180/M_PI >0)
       g = & gfrl[1][0];             // NW
    else 
      g = & gfrl[7][0];              // NE   
  }else  if ( -65 > m && m >= -110  ) 
    g = & gfrl[0][0];                // N
  else
    // we can disregard this reading no big deal
    //PRINTERR("Unexpected reading from MPU6050");
  for(int i=0; i< mCountl; i++){
    mList[i]->gravityChanged(g);
  }
}


void Grav::registerListener(GListener& gl){
  if(mCountl<GRAV_LISTENER_COUNT)
    mList[mCountl++] = &gl;
  else
    PRINTERR("Unexpected gravity listener trying to register");
}


Ball::Ball(){
#ifdef PCRUN  
 int rollside = (rand()%2 ? ROLLRIGHT : ROLLLEFT );
#else
  int rollside = (random(0,2) ? ROLLRIGHT : ROLLLEFT );
#endif
  setRoll(rollside);
}


inline void Ball::setRoll(int rollside){
  rollside = (rollside == ROLLLEFT ? 0 : 1);
  mFlags.val ^= (-rollside ^ mFlags.val) & (1 << ROLLSIDEBIT);
}

inline int Ball::getRoll(){
  //if bit set to 1 roll RIGHT , else (bit set to 0)  roll LEFT
  return ((mFlags.val >> ROLLSIDEBIT) & 1 ) ? ROLLRIGHT : ROLLLEFT;
}

inline bool Ball::isRolling(){
  return ((mFlags.val >> ROLLBIT) & 1 ) ? true : false;
}

inline void Ball::setRolling(bool roll){
  int x = ( roll ? 1 : 0 );
  mFlags.val ^= (-x ^ mFlags.val) & (1 << ROLLBIT);
}

inline bool Ball::isFalling(){
  return ((mFlags.val >> FALLBIT) & 1 ) ? true : false;
}

inline void Ball::setFalling(bool fall){
  int x = ( fall ? 1 : 0 );
  mFlags.val ^= (-x ^ mFlags.val) & (1 << FALLBIT);
}


void Ball::fall(Locations& gLoc, Grav& gE){
  if (gLoc.isAvailable(gE.g[FALL][Y]+mR, gE.g[FALL][X] + mC)){
    setFalling(true);
    if (++mFcounter > FINERT){
      mFcounter = 0;
      int r = mR;
      int c = mC;
      mR += gE.g[FALL][Y];
      mC += gE.g[FALL][X];
      gLoc.moveBallFrom(r, c);
      setFalling(false);
    }
  }else if (!isFalling()){
    int rollR = getRoll();
#ifdef PCRUN    
    if(!isRolling() && (rand()%2 == 1)){
#else
    if(!isRolling() && (random(0,2) == 1)){
#endif      
      rollR = -1*rollR;
      setRoll(rollR);
    }
    int ridx = (rollR == 1 ? RIGHT : LEFT); 
    if (gLoc.isAvailable(gE.g[ridx][Y]+mR, gE.g[ridx][X]+mC)){
      setRolling(true);
      if (++mFcounter > RINERT){
	mFcounter =0;
	int r = mR;
	int c = mC;
	mR += gE.g[ridx][Y];
	mC += gE.g[ridx][X];
	gLoc.moveBallFrom(r, c);
	setRolling( false);
      }
    }
  }else{
    setFalling(false);
    setRolling(false);
    mFcounter = 0;
  }
}

void Ball::setRC(int r, int c){
  mR = r;
  mC = c;
}


Locations::Locations(Grav& grav):mGrav(grav){
  mGrav.registerListener(*this);
  for(int r=0; r <= MAX_ROW; r++){
    for (int c=0; c<= MAX_COL; c++){
      mBalls[r][c] = nullptr;
    }
  }
  for(int i=0; i<BALLCOUNT; i++){
    placeBall(sand[i]);
    updateView(-1, -1, sand[i].row(), sand[i].col());
  }
  PRINTERR("loc all balls placed in locations");
}


void Locations::updateView(int oldr, int oldc, int r, int c){
  if(oldr <0 ){
    //do nothing, this means we are placing the ball first time
  }else{
    //turn older location's bit off
    ballView[oldr] &= ~(1 << oldc);
  }
  //turn the new location bit on
  ballView[r] |= 1 << c;  
}


void Locations::gravityChanged(const char (*g)[2]){
  //TODO does this still have  any use?
  // keep for later
}


void Locations::placeBallNextAvailable(Ball& ball){
  for(int i=0; i + ball.row() <= MAX_ROW; i++){
    for(int j=0; j + ball.col() <= MAX_COL; j++){
      if ( isAvailable(ball.row() + i, ball.col() +j) ){
	ball.setRC( ball.row() + i, ball.col() + j);
	mBalls[ball.row()][ball.col()] = &ball;
	return;
      }
    }
  }
  PRINTERR("Unexpected , cannot find a place for the ball");
}


void Locations::placeBall(Ball& ball){
  if ( isAvailable(ball.row(), ball.col()) )
    mBalls[ball.row()][ball.col()] = &ball;
  else
    placeBallNextAvailable( ball);
}

void Locations::moveBallFrom(int r, int c){
  Ball* b = mBalls[r][c];
  if( b != nullptr){
    mBalls[r][c] = nullptr;
    mBalls[b->row()][b->col()] = b;
    updateView(r, c, b->row(), b->col());
  }else{
    PRINT("unexpected ball location ");
  }  
}

void Locations::applyGravity(const char (*g)[2]){
  // start scanning based on gravity, TBD
  if(g[FALL][Y] < 0){
    for(int r=0; r <= MAX_ROW; r++){
      for (int c=0; c <= MAX_COL; c++){
	if(hasBall(r,c)){
	  mBalls[r][c]->fall(*this, mGrav);
	}
      }
    }
  }else if (g[FALL][Y] >= 0){
    for(int r = MAX_ROW; r >= 0; r--){
      for (int c=MAX_COL; c >= 0; c--){
	if(hasBall(r,c)){
	  mBalls[r][c]->fall(*this, mGrav);
	}
      }
    }
  }
}


//for PC TEST only 
#ifdef PCRUN
void Locations::display(){
  PRINT("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
  const char* g = SYM_MASK;
  PRINT(g << g << g <<g << g << g <<g << g << g <<g << "\n");
  for (int r=0; r <= MAX_ROW; r++){
    PRINT(g);
    for (int c=0; c <= MAX_COL; c++){
      const char* wc =" ";
      if(isMask(r, c)){
	wc = SYM_MASK;
      }else if (hasBall(r, c)) {
	wc = SYM_BALL;
      }
      PRINT(wc);	
    }
    PRINT(g<<" "<<r<<"\n");
  }
  PRINT(g << g << g <<g << g << g <<g << g << g <<g);
  if (mGrav.g[FALL][Y] >0)
    PRINT("\u2193\n"); // down arrow
  else
    PRINT("\u2191\n");  // up arrow
  PRINT("\n");  
}
#endif


