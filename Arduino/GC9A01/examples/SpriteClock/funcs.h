// Function definitions

uint16_t getStrRenderLength(char *s, uint16_t limitLength=9999);
void drawStr(char *s,int16_t x, int16_t y, uint16_t limitLength=9999);
void drawSpike(float angle);
void showWelcomeMessage();
void showMemoryStatus();
void updateNumerals(uint32_t hms);
void doGhostAnim();

// Function implementations

uint16_t getStrRenderLength(char *s, uint16_t limitLength){
  uint8_t ix;
  uint16_t l=0;
  while(*s!=0 && (limitLength-->0)){
    ix=(*s++);
    if(ix==' '){
      l+=8;
    }else if(ix<='9'){
      l+=charWidth[ix-'0']+1;
    }else{
      l+=charWidth[(ix-'a')+10]+1;
    }
  }
  return l;
}

void drawStr(char *s,int16_t x, int16_t y, uint16_t limitLength){
  uint8_t ix;
  while(*s!=0 && (limitLength-->0)){

    ix=(*s++);
    if(ix==' '){
      ix=36;
    }else if(ix<='9'){
      ix-='0';
      display->drawSpriteImg(&charImg,x,y,ix);
    }else{
      ix=(ix-'a')+10;
      display->drawSpriteImg(&charImg,x,y,ix);
    }
    x+=charWidth[ix]+1;
  }
}




void drawSpike(float angle,float spikeLength){

  int16_t r=spikeLength*8*3;
  if(r>255)
    r=255;
    
  int16_t g=0;
  if(spikeLength>10){
    g=(spikeLength-10)*26;
    if(g>255)
      g=255;
  }
    
  int16_t b=255-(spikeLength*5*3);
  if(b<0){
    b=(-b*4);
    if(b>255)
      b=255;
  }

  float radOff=120;
  
  uint16_t c=display->getColor16(r,g,b);
  const float spikeWidth=3;

  float x=120+(sin(angle)*radOff);
  float y=120+(-cos(angle)*radOff);

  int16_t p0x=x+(-sin(angle)*spikeLength);
  int16_t p0y=y+(cos(angle)*spikeLength);

  int16_t p1x=x+(-sin(angle+(PI/2))*spikeWidth);
  int16_t p1y=y+(cos(angle+(PI/2))*spikeWidth);

  int16_t p2x=x+(-sin(angle+(PI*1.5))*spikeWidth);
  int16_t p2y=y+(cos(angle+(PI*1.5))*spikeWidth);

  display->fillTriangle(
    p0x,p0y,p1x,p1y,p2x,p2y,c
  );
  
}

void showWelcomeMessage(){
  // Fade display to black at boot
  for(int16_t n=255;n>=0;n-=5){
    display->clearBuffer(display->getColor16(n,n,n));
    display->drawFrame();
  }

  // Now show a welcome message (fade and scroll to centre)
  uint16_t y=20;
  uint16_t e=0;
  int16_t yDir=-1;
  uint16_t s0X=(240-getStrRenderLength("hello"))/2;
  uint16_t s1X=(240-getStrRenderLength("world"))/2;
  uint16_t delay=100;
  float accel=(PI*2)/360;
  float rot=0;
  float spikeOpacity=3;
  float spikeLength=10;
  const uint16_t numSpikes=6;
  const float spacing=(PI*2)/numSpikes;
  const float trailSpacing=(spacing*0.07);

  // This is the slowest draw method - but allows nice effects. If the mergeAmount is set to 16 it's the same performance as the overwrite
  // render type. If the mergeAmount is set to zero, nothing is drawn.
  // This copies the default renderBuffer to the physical display
  display->setRenderType(RENDER_TYPE_MERGE);

  lMillis=millis();
  while(y!=e){
    cMillis=millis();
    if(delay==0)
      y+=yDir;
    else
      --delay;
    // Fill the current renderBuffer with the specified color (black here)
    // Note that this method always does an overwrite - it ignores the renderType setting
    display->clearBuffer(0);

    display->setMergeAmount(20-y);

    drawStr("hello",s0X,100-y);
    drawStr("world",s1X,120+y);

    for(uint16_t n=0;n<numSpikes;n++){
      uint16_t spikeOp=(uint16_t)spikeOpacity;
      while(spikeOp>=1){
        display->setMergeAmount(spikeOp-=0.05);
        drawSpike((spikeOp*trailSpacing*accel*3.0)+rot+(spacing*n),spikeLength-((16-spikeOp)/2));
      }
    }
    if(yDir==-1){
      if(spikeOpacity<16)
        spikeOpacity+=0.2;
    }else{
      if(spikeOpacity>0){
        spikeOpacity-=1;
      }
    }
      
    rot+=accel;
    if(accel<spacing*0.10)
      accel+=0.001;
      
    if(spikeLength<20)
      spikeLength+=0.2;

    display->drawFrame();
    
    if((e==y) && (e==0)){
      y=1;
      if((cMillis-lMillis)>5000){
        y=0;
        yDir=1;
        e=20;
      }
    }
  }
  display->clearBuffer(0);
  display->drawFrame();
}

void showMemoryStatus(){
  // Just show free memory for debugging purposes
  Serial.printf("Total heap:  %d\r\n", ESP.getHeapSize());
  Serial.printf("Free heap:   %d\r\n", ESP.getFreeHeap());
  Serial.printf("Total PSRAM: %d\r\n", ESP.getPsramSize());
  Serial.printf("Free PSRAM:  %d\r\n", ESP.getFreePsram());
}

// Accepts time in 24 hour format as HHMMSS in a single 32 bit uint. So 12:34:56 = 123456
void updateNumerals(uint32_t hms){
  uint16_t fPosIX=0,tPosIX=0,spriteIX=0;
  uint32_t inHms=hms;
  for(int16_t d=NUM_CLOCK_DIGITS-1;d>=0;d--){
    uint8_t hmsDiv=hms%10;
    uint8_t lHmsDiv=lastHms%10;
    hms/=10;
    lastHms/=10;
    if(hmsDiv!=lHmsDiv){
      const uint8_t *fp=nPos[hmsDiv][0];
      const uint8_t *tp=nPos[hmsDiv][1];
      uint16_t moveDelay=((NUM_CLOCK_DIGITS-1)-d)*10;
      
      for(uint16_t dPos=0;dPos<DIGIT_HEIGHT*DIGIT_WIDTH;dPos++){
        fPosIX=(*(fp+dPos))+(15*d);
        tPosIX=(*(tp+dPos))+(15*d);
        spriteIX=dPos+(15*d);
        pacmanOb[spriteIX]->setSpeed(1);
        pacmanOb[spriteIX]->setPos(digitPosition[fPosIX]->x,digitPosition[fPosIX]->y);
        pacmanOb[spriteIX]->setTargetPos(digitPosition[tPosIX]->x,digitPosition[tPosIX]->y,moveDelay);
      }
    }
  }
  lastHms=inHms;
//  while(1){delay(1000);}
}

void drawCentredStr(char *s, int16_t y){
  uint16_t s0X=(240-getStrRenderLength(s))/2;
  drawStr(s,s0X,y);
}

void doGhostAnim(){

  if(ghostAnimState==0){  // Should check ms since last alery here...
    ghostAnimState=1;
    ghostX=-72;
    if(++fakeAlertIX==5)
      fakeAlertIX=0;
      if(++ghostPalette==4)
        ghostPalette=0;
  }
  if(ghostAnimState==1 || ghostAnimState==2){
    display->setRenderType(RENDER_TYPE_OVERWRITE);
    drawCentredStr(fakeAlert[fakeAlertIX][0],170);
    drawCentredStr(fakeAlert[fakeAlertIX][1],190);
  
    display->setRenderType(RENDER_TYPE_MERGE);
    for(uint16_t n=0;n<15;n++){
      display->setMergeAmount(15-n);
      if(ghostAnimState==1){
        display->fillRect(ghostX-(2*n),150,ghostX-(2*n)+2,220,0);
      }else{
        display->fillRect(ghostX+80+(2*n),150,ghostX+82+(2*n)+2,220,0);
      }
    }
    display->setRenderType(RENDER_TYPE_OVERWRITE);
    if(ghostAnimState==1){
      display->fillRect(ghostX+2,150,240,223,0);  // Partial clear screen
    }else{
      display->fillRect(ghostX-300,150,ghostX+80,223,0);  // Partial clear screen
    }
    ghostX+=3;
    if(ghostX>300){
      if(ghostAnimState==1){
        ghostAnimState=2;
        ghostX=-72;
      }else{
        ghostAnimState=0;
      }
    }
    
    if(cMillis-ghostAnimMS>=300){
      ghostAnimMS+=300;
      ghostFrame=(ghostFrame==0?1:0);
    }
    display->setRenderPaletteIndex(ghostPalette);
    display->drawSpriteImg(&ghostGfx,ghostX,150,ghostFrame);
    display->setRenderPaletteIndex(0);
  }
}
