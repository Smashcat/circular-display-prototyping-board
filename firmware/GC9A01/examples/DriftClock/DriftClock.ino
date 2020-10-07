/**
 * Clock demo 1, with popups
 * 
 * Hardware: ESP32, GC9A01 240x240 circular display
 * Author: scott@smashcat.org
 * 
 */
#include <SPI.h>
#include <GC9A01.h>

#include "data.h"
#include "vars.h"
#include "funcs.h"

void setup() {
  // Can either use VSPI or HSPI hardware peripheral. We pass it into the display class so it can also be used outside, for other purposes if needed
  vspi = new SPIClass(VSPI);
  vspi->begin();

  // Initialise the display and turn it on
  display=new GC9A01(vspi);
  // Note that thsi GC9A01 controller initialises with all pixels set to 0xFFFF (white) so here we clear it to black immediately after configuring it
  display->clearBuffer(0);
  // Now update the physical display with the contents of the render buffer
  display->drawFrame();

  // Just for debugging, show current memory usage after creating display buffer
  Serial.begin(115200);
  showMemoryStatus();
  
  // Pre-calculate positions of numerals on clock dial, relative to the centre
  for(uint16_t n=0;n<12;n++){
    nX[n]=(-sin(2*PI/12*(n-5))*radius);
    nY[n]=(cos(2*PI/12*(n-5))*radius);
    nX[n]-=(numeral[n].frameWidth/2);
    nY[n]-=(numeral[n].frameHeight/2);
  }
  
  showWelcomeMessage();
}

void loop() {

  // This is the fastest draw method - no blending of src and dest pixels
  display->setRenderType(RENDER_TYPE_OVERWRITE);

  // Clear the renderBuffer to this color (ignores the renderType setting - this ALWAYS overwrites)
  display->clearBuffer(0xB5B6);

  cMillis=millis()*2; // Just double the speed of the second updates, to make it more interesting to watch :-)
  cSec=(float)((cMillis-sMillis)/60000.0);

  // Update the hour/minute values if a minute has elapsed
  if(cSec>=1){
    Serial.printf("%02i:%02i\n",cHour,cMin);
    sMillis=cMillis;
    if(++cMin==60){
      cMin=0;
      if(++cHour==12)
        cHour=0;
    }
    cSec=0;
  }

  // Position centre of dial based upon seconds
  x=120+sin((2*PI)*(cSec+0.5))*rotationRadius;
  y=120-cos((2*PI)*(cSec+0.5))*rotationRadius;

  // Fake date on display
  drawStr("1st October 2020",x-50,y-170);

  // Draw the numerals around dial
  for(uint16_t n=0;n<12;n++){
    display->drawSpriteImg(&numeral[n],(int16_t)(x+nX[n]),(int16_t)(y+nY[n]),0);
  }

  // A nice cat at the bottom of the dial :-)
  display->drawSpriteImg(&catImg,x-40,y+80,0);

  // Shadow color for hands is drawn first
  const uint16_t sC=display->getColor16(40,40,40);
  display->setRenderType(RENDER_TYPE_MERGE);

  // Opacity is set to 4/16 (25%)
  display->setMergeAmount(4);
  drawHand(x+5,y+5,(2*PI/60)*((cHour+cMin/60.0)*5),radius*0.60,sC);
  drawHand(x+5,y+5,(2*PI/60)*(cMin+cSec),radius*0.85,sC);
  drawHand(x+5,y+5,(2*PI/60)*((uint16_t)(cSec*60)),radius*0.95,sC);

  // For main hands in front of shadows, we set opacity to 12 (75%)
  display->setMergeAmount(12);
  drawHand(x,y,(2*PI/60)*((cHour+cMin/60.0)*5),radius*0.60,0x2945);
  drawHand(x,y,(2*PI/60)*(cMin+cSec),radius*0.85,0x4A49);
  drawHand(x,y,(2*PI/60)*((uint16_t)(cSec*60)),radius*0.95,0x7bcf);

  // Animate popup if necessary
  handlePopup();

  // copy the current rendered image to the physical display
  display->drawFrame();

  // Every 60 frames we display the FPS
  if(++cnt==maxFrames){
    cnt=0;
    Serial.printf("time for %i frames: %i = %i FPS\r\n",maxFrames,(cMillis-ms)/2,1000/(((cMillis-ms)/2)/maxFrames)); // Divide by 2 as we double the millis to speed up the demo
    ms=cMillis;
  }
}
