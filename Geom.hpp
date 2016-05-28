#ifndef _GEOM_HPP
#define _GEOM_HPP

#include "macros.h"

#define MATRIX_ROW 3
#define MATRIX_COL 1
#define MATRIX_SIZE 8
#define MATRIX_COUNT (MATRIX_ROW*MATRIX_COL)
#define LOC_COUNT (MATRIX_COUNT*MATRIX_SIZE*MATRIX_SIZE)
#define MIN_ROW 0
#define MIN_COL 0
#define MAX_ROW ((MATRIX_ROW*MATRIX_SIZE)-1)
#define MAX_COL (MATRIX_SIZE -1)
#define FALL 0  //gravity array index
#define RIGHT 1 //gravity array index
#define LEFT 2  //gravity array index
#define X 0     //gravity array index
#define Y 1     //gravity array index

#define ROLLLEFT -1
#define ROLLRIGHT 1
#define FALLBIT 0   // Ball state bits in order
#define ROLLBIT 1
#define ROLLSIDEBIT 2
#define SYMBIT 3

#define BALLCOUNT 60   //number of balls to display
#define GRAV_LISTENER_COUNT 1  // only locations obj to listen
#define FINERT 5  // ball inetrtia before falling
#define RINERT 2  //ball inetrtia before rolling


namespace ldm {

  typedef struct {
    unsigned char val;
  }flags_t;  // Ball state flags
  
#ifdef PCRUN  // only needed for PC display simulation  
  extern const char* const SYM_BALL;
  extern const char* const SYM_MASK;
#endif

  
  /*
   * Anything that needs to be informed gravity changed 
   * should extend this abstract class and
   * register itself with gravity as a listener
   */
  class GListener{
  public:
    virtual void gravityChanged(const char (*g)[2]) = 0;
  };

  /* 
   *  This class represents gravity and lets listeners know 
   *  about changes in gravity
   *
   */
  class Grav{
  public:
    Grav();
    void updateGravity(int m);  //sensor measurement m is passed here
    void registerListener(GListener& gl);
    int countl() const; // number of gravity listeners
    //current gravity values pointer is g* , this determines how balls fall
    const char (*g)[2];
    // all possible gravity values gfrl, for 8 compass directions 
    static const char gfrl[][3][2]; 
  private:
    GListener* mList[GRAV_LISTENER_COUNT]; 
    unsigned char mCountl =0;
    int mPreviousM = 0;
  };

  
  class Locations;
  

  class Ball{
  public:
    friend Locations;
  private:
    Ball();
    void fall(Locations& gLoc, Grav& gE);
    inline int row() const { return mR;};
    inline int col() const {return mC;}
    void setRC(int r, int c);
    bool isFalling();
    void setFalling(bool);
    bool isRolling();
    void setRolling(bool);
    int  getRoll(); //roll side left or right
    void setRoll(int);
    //initial flag settings are
    // Fall true, Roll False, Rollside Right
    flags_t mFlags ={.val = 0b00000101};
    char mFcounter = 0;
    unsigned char mR = 0;
    unsigned char mC = 0;
  };

  
  class Locations: public GListener{
  public:
    Locations(Grav& grav);
    virtual void gravityChanged(const char (*g)[2]) override;    
    inline bool isMask(int r, int c) const{
      return ((HGMASK[r] >> c) & 1 ) ? true : false;
    };
    //inline bool hasBall(int r, int c) const {
    //  return  mBalls[r][c] ? true : false;
    //};
    inline bool hasBall(int r, int c) const {
      return  (ballView[r] >> c & 1) ? true : false;
    };
    inline bool isFull(int r, int c) const {
      return ( isMask(r,c) || hasBall(r,c) ? true : false);
    };
    inline bool isAvailable(int r, int c) const{
      return !(isFull(r,c) ||
	       r > MAX_ROW || r < 0 || c > MAX_COL || c< 0); 
    };    
    inline unsigned char viewData(int row){ return ballView[row];};
    void placeBall(Ball& ball);
    void moveBallFrom(int r, int c);
    void applyGravity(const char (*g)[2]);
#ifdef PCRUN
    void display(); // for PC TEST only
#endif

  private:
    void placeBallNextAvailable(Ball& ball);
    void updateView(int oldr, int oldc, int r, int c);
    Ball sand[BALLCOUNT];
    Ball* mBalls[MAX_ROW+1][MATRIX_SIZE];
    unsigned char ballView[MAX_ROW+1]={0};
    Grav& mGrav;
    static const unsigned char HGMASK[];
  };


  
}//end of name space ldm


#endif  //end of ifdef _GEOM_HPP
