/**
 * Sprite scaling demo
 * 
 * Hardware: ESP32, GC9A01 240x240 circular display
 * Author: scott@smashcat.org
 * 
 * This demo shows how to use the sprite scaling function
 */
#include <SPI.h>

// Production library include is below - if you're just using the library in your project, and not developing it, just use the below instead!
//#include <GC9A01.h>

// Dev include
#include "Z:\Eagle projects\CircularTouchscreenTest\Arduino\GC9A01\src\GC9A01.h"
#include "Z:\Eagle projects\CircularTouchscreenTest\Arduino\GC9A01\src\GC9A01.cpp"


#include "defs.h"
#include "data.h"
#include "vars.h"
#include "funcs.h"

void setup() {

  Serial.begin(115200);

  // Can either use VSPI or HSPI hardware peripheral. We pass it into the display class so it can also be used outside, for other purposes if needed
  vspi = new SPIClass(VSPI);
  vspi->begin();

  // Initialise the display and turn it on
  display=new GC9A01(vspi);

  for(uint16_t n=0;n<NUM_GHOSTS;n++){
    ghostOb[n]=new GhostOb();
  }

  showWelcomeMessage();

}

void loop() {
  cMillis=millis();
//  display->setRenderType(RENDER_TYPE_OVERWRITE);
//  display->clearBuffer(0);
  display->setRenderType(RENDER_TYPE_MERGE);
  display->setMergeAmount(5);
  display->fillRect(0,0,240,240,0);

  for(uint16_t n=0;n<NUM_GHOSTS;n++){
    ghostOb[n]->update();
    ghostOb[n]->draw();
  }

  display->setRenderPaletteIndex(0);

  updateTextWriter();
  
  // copy the current rendered image to the physical display
  display->drawFrame();

  // Every 60 frames we display the FPS
  if(++cnt==maxFrames){
    cnt=0;
    Serial.printf("time for %i frames: %i = %i FPS\r\n",maxFrames,(cMillis-lMillis),1000/((cMillis-lMillis)/maxFrames));
    lMillis=cMillis;
  }
}
