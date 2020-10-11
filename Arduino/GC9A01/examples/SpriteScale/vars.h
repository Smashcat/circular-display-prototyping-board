SPIClass *vspi=NULL;
GC9A01 *display;
GC9A01::RenderBuffer *rb;

// Simple sprite object class to draw the floating ghosts
class GhostOb {
  
  public:

    float   x=0,y=0,z=0,speed=0;
    uint16_t palIX=0;
    
    GhostOb(){
      resetPos();
    }

    void resetPos(){
      x=random(0,2000)-1000;
      y=random(0,2000)-1000;
      z=30+random(40);
      speed=(-0.01*((float)random(10,100)));
      animFrame=0;
      animDelay=10+random(10);
      animCnt=animDelay;
      palIX=random(0,4);
    }

    void setSpeed(float s){
      speed=s;
    }
    
    boolean update(void){
      if(--animCnt==0){
        animCnt=animDelay;
        if(++animFrame==2)
          animFrame=0;
      }
      z+=speed;
      if(z<1){
        resetPos();
      }
    }

    void draw(void){
      // Always draw it
      if(z>0){
        int16_t x3=(x/z);
        int16_t y3=(y/z);
        uint16_t w3=(80/z);
        uint16_t h3=(73/z);
        int16_t xPos=120+x3-(w3/2);
        int16_t yPos=120+y3-(h3/2);
        int16_t op=14-(z/2);
        if(op<0)
          op=0;
        display->setMergeAmount(op);
        display->setRenderPaletteIndex(palIX);
        display->drawSpriteImg(&ghostGfx,xPos,yPos,w3,h3,animFrame);
      }
    }
    
  private:
    uint16_t  animFrame=0,animDelay=0,animCnt=0;
};


GhostOb * ghostOb[NUM_GHOSTS];

// hold x,y positions
class Coord {
  public:
    int16_t x,y;
    Coord(int16_t pX,int16_t pY){
      x=pX;
      y=pY;
    }
};

// Vars for ghost
  uint16_t w=4;
  uint16_t h=4;
  int16_t wDir=4;
  int16_t hDir=4;
  uint16_t op=0;
  uint16_t spd=4;
  int16_t opDir=spd;


// Vars for animated text writer
const char * writerTxt[]={
  "Hello world!",
  "Sprite scaling example",
  "Pretty blurry huh?!",
  "GFX use depth opacity",
  "Library is on Github",
  "(Under development)",
  "Have a nice day!",
};

uint16_t writerPause=10;
uint16_t stringIX=1;
uint16_t stringPos=0;
float stringSubPos=1;
uint16_t wop=0;

// For FPS counter on serial port
unsigned long cMillis=0;
unsigned long lMillis=0;
uint16_t cnt=0;
const uint16_t maxFrames=60;
