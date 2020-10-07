SPIClass *vspi=NULL;

// Used for framerate counter on serial port
unsigned long ms=0;
uint16_t cnt=0;
const uint16_t maxFrames=60;

// Stores position of numerals around dial
float nX[12]={0};
float nY[12]={0};

// Used to draw dial each frame
int16_t pos=0;
const uint16_t radius=220;
const uint16_t rotationRadius=130;
const float handWidth=0.1;
float x=120.0,y=120.0,cSec=0;
uint16_t cMin=50,cHour=11;
unsigned long sMillis=0,cMillis=0;

// Used to animate the popup over dial
float popupY=240;
uint16_t popupTxtLen=0;
uint16_t popupCnt=0;
uint16_t popupState=0;  // 0 = off, 1 = showing, 2 = on, 3 = hiding - just a hack for demo, obviously this isn't actually usable code ;-)
uint16_t popupMsgIX;
char popupStr[3][18]={
  "NEW EMAIL",
  "FACEBOOK MSG",
  "MEETING IN 5 MINS"
};

// The display object, only one of these needed per sketch
GC9A01 *display;
