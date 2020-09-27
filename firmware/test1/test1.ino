
#include <SPI.h>
#include <GC9A01.h>

SPIClass *vspi=NULL;

unsigned long ms=0;
unsigned long cMillis=0;
uint16_t cnt=0;
const uint16_t maxFrames=60;
int16_t amp=80;
int16_t ampDir=-1;
uint32_t cFrame=0;
GC9A01 *display;

uint16_t colors[9]={
  
  (15<<11)+(0<<5)+0,
  (0<<11)+(30<<5)+0,
  (0<<11)+(0<<5)+15,

  (20<<11)+(40<<5)+20,
  (15<<11)+(30<<5)+15,
  (10<<11)+(20<<5)+10,

  (15<<11)+(30<<5)+0,
  (0<<11)+(30<<5)+15,
  (15<<11)+(0<<5)+15,
};

void setup() {
  vspi = new SPIClass(VSPI);
  vspi->begin();
  display=new GC9A01(vspi);
  Serial.begin(115200);
}

void loop() {
  display->clearBuffer(0);
  uint8_t ix=0;
  int16_t fX,fY;
  float fA;
  const uint16_t numRects=15;
  amp+=ampDir;
  if(amp==-100)
    ampDir=1;
  else if(amp==100)
    ampDir=-1;
  for(ix=0;ix<numRects;ix++){
//  for(uint16_t y=0;y<3;y++){
//    for(uint16_t x=0;x<3;x++){
      fA=( ( (PI*2)/numRects)*ix ) + (cFrame*0.12);
      fX=(sin(fA)*amp);
      fY=(cos(fA)*amp);
//      display->fillRect(50+(x*50),50+(y*50),50+(x*50)+40,50+(y*50)+40,colors[ix],RENDER_TYPE_XOR);
      display->fillRect(90+fX,90+fY,150+fX,150+fY,colors[ix%9],RENDER_TYPE_ADDITIVE);
//    }
  }
  display->drawFrame();
  if(++cnt==maxFrames){
    cMillis=millis();
    cnt=0;
    Serial.printf("time for %i frames: %i\r\n",maxFrames,cMillis-ms);
    ms=cMillis;
  }
  ++cFrame;
}
