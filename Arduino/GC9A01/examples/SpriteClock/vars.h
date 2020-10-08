SPIClass *vspi=NULL;
GC9A01 *display;
GC9A01::RenderBuffer *rb;

// Simple sprite object class, to draw all the Pacman characters that form the clock
class PacmanOb {
  
  public:

    int16_t   x=0,y=0;

    PacmanOb(int16_t pX,int16_t pY){
      targetX=x=pX;
      targetY=y=pY;
      isActive=false;
    }

    void setPos(int16_t pX, int16_t pY){
      targetX=x=pX;
      targetY=y=pY;
      //display->drawSpriteImg(&pacmanGfx,(int16_t)x,(int16_t)y,baseFrame+animFrame);
    }

    void setTargetPos(int16_t pX, int16_t pY, uint16_t aD){
      if((pX==x && pY==y)||(pX==targetX && pY==targetY)){
        return;
      }
      targetX=pX;
      targetY=pY;
      animDelay=aD;
      isActive=true;
    }

    void setSpeed(float s){
      speed=s;
    }
    
    boolean update(void){

      if(isActive){
        if(animDelay>0){
          --animDelay;
        }else{
          animFrame+=animDir;
          
          if(animFrame==5)
            animDir=-1;
          else if(animFrame==0)
            animDir=1;
            
          if(x<targetX){
            baseFrame=12;
            x+=speed;
            if(x>targetX)
              x=targetX;
          }else if(x>targetX){
            baseFrame=18;
            x-=speed;
            if(x<targetX)
              x=targetX;
          }else if(y<targetY){
            baseFrame=6;
            y+=speed;
            if(y>targetY)
              y=targetY;
          }else if(y>targetY){
            baseFrame=0;
            y-=speed;
            if(y<targetY)
              y=targetY;
          }else if(animFrame!=0){
            
          }else{
            animFrame=0;
            animDir=1;
            isActive=false;
          }
        }
      }
      
      // Always draw it
      display->drawSpriteImg(&pacmanGfx,(int16_t)x,(int16_t)y,baseFrame+animFrame);
      return isActive;
    }
    
  private:
    uint16_t  baseFrame=0,animFrame=0,animDelay=0;
    int16_t   animDir=1;
    boolean   isActive=false;
    int16_t   targetX,targetY;
    int16_t   speed=1;
};

// hold x,y positions
class Coord {
  public:
    int16_t x,y;
    Coord(int16_t pX,int16_t pY){
      x=pX;
      y=pY;
    }
};

// Position of numerals on display
const uint16_t digitPos[NUM_CLOCK_DIGITS]={7,42,88,123,167,202};
const uint16_t digitPosY=98;

// Position of dots on display
const Coord * dotPos[NUM_DOTS]={
  new Coord(76,109),
  new Coord(156,109),
  new Coord(76,129),
  new Coord(156,129)
};
unsigned long lDotMillis=0;
uint16_t dotFrame=0;

uint8_t clockHr=23;
uint8_t clockMin=59;
uint8_t clockSec=31;
uint32_t lastHms=0;
unsigned long cSec=0;

// For FPS counter on serial port
unsigned long cMillis=0;
unsigned long lMillis=0;
uint16_t cnt=0;
const uint16_t maxFrames=60;

unsigned long frameLimiter=0;
unsigned long ghostAnimMS=0;
uint16_t ghostFrame=0;
int16_t ghostX=83;
uint16_t ghostPalette=0;
uint16_t fakeAlertIX=0;
uint16_t pacmanOpacity=0;
uint16_t ghostAnimState=0;

char fakeAlert[5][2][10]={
  {"time","flies"},
  {"facebook","message"},
  {"incoming","call"},
  {"instagram","message"},
  {"meeting","3pm"},
};


PacmanOb * pacmanOb[NUM_PACMAN_SPRITES];
Coord * digitPosition[NUM_PACMAN_SPRITES];
