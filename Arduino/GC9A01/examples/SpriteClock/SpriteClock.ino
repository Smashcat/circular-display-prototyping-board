/**
 * Clock demo 2, using sprites
 * 
 * Hardware: ESP32, GC9A01 240x240 circular display
 * Author: scott@smashcat.org
 * 
 * This demo shows how to use a second RenderBuffer to quickly copy sprites to the physical screen for faster updates
 */
#include <SPI.h>

// Production library include is below - if you're just using the library in your project, and not developing it, just use the below instead!
#include <GC9A01.h>

#include "defs.h"
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

  Serial.begin(115200);

  // Ok, so normal sprite drawing to the main renderBuffer is pretty quick, but we can speed things up considerably by drawing sprites to smaller
  // RenderBuffers, to blit directly to the physical device - this is a partial screen update, changing only the pixels needed! There are some
  // Restrictions - you cannot do any kind of blending or transparency with the existing pixels on the display, you can only overwrite, but this
  // may not be a problem in a lot of cases. It's also possible to get screen tearing if you update at very high speeds.
  rb=new GC9A01::RenderBuffer(240,50);

  showWelcomeMessage();

  // Fake the top date display for the demo
  display->setRenderType(RENDER_TYPE_MERGE);
  for(uint16_t n=0;n<17;n++){
    display->clearBuffer(0);
    display->setMergeAmount(n);
    display->drawSpriteImg(&fakeDateGfx,63,34,0);
    display->drawFrame();
  }

  display->setRenderType(RENDER_TYPE_OVERWRITE);

  // Create an array of digit positions for clock
  // Also create array of pacman sprites that draw the clock :-)
  uint16_t spriteIX=0;
  for(uint16_t d=0;d<NUM_CLOCK_DIGITS;d++){
    for(uint16_t y=0;y<DIGIT_HEIGHT;y++){
      for(uint16_t x=0;x<DIGIT_WIDTH;x++){
//        pacmanOb[spriteIX]=new PacmanOb(digitPos[d]+(x*10)+230,digitPosY+(y*10)+100);
        digitPosition[spriteIX]=new Coord(digitPos[d]+(x*10),digitPosY+(y*10));
        pacmanOb[spriteIX]=new PacmanOb(digitPosition[spriteIX]->x,digitPosition[spriteIX]->y);
        pacmanOb[spriteIX]->setSpeed(5);
        pacmanOb[spriteIX]->setTargetPos(digitPosition[spriteIX]->x,digitPosition[spriteIX]->y,0);  // (d*20)+(y*5)+x
        ++spriteIX;
      }
    }
  }
}

void loop() {
  cMillis=millis();
  
  // This is the fastest draw method - no blending of src and dest pixels
  display->setRenderType(RENDER_TYPE_OVERWRITE);
  display->fillRect(7,98,233,150,0);  // Partial clear screen

  if(cMillis-cSec>=1000){
    cSec+=1000;
    lDotMillis=cMillis;
    dotFrame=5;
    if(++clockSec==60){
      clockSec=0;
      if(++clockMin==60){
        clockMin=0;
        if(++clockHr==24){
          clockHr=0;
        }
      }
    }
    updateNumerals((clockHr*10000)+(clockMin*100)+clockSec);
  }
  
  display->setRenderType(RENDER_TYPE_MERGE);
  if(pacmanOpacity<16){
    ++pacmanOpacity;
  }
  display->setMergeAmount(pacmanOpacity);
  for(uint16_t n=0;n<NUM_PACMAN_SPRITES;n++){
    pacmanOb[n]->update();
  }

  doGhostAnim();

  if(cMillis-lDotMillis>=150){
    lDotMillis+=150;
    if(++dotFrame>=6)
      dotFrame=0;
  }
  for(uint16_t n=0;n<NUM_DOTS;n++){
    display->drawSpriteImg(&dotGfx,dotPos[n]->x,dotPos[n]->y,dotFrame);
  }
  
  // copy the current rendered image to the physical display
  display->drawFrame();

  // Every 60 frames we display the FPS
  if(++cnt==maxFrames){
    cnt=0;
    Serial.printf("time for %i frames: %i = %i FPS\r\n",maxFrames,(cMillis-lMillis),1000/((cMillis-lMillis)/maxFrames));
    lMillis=cMillis;
  }
}
