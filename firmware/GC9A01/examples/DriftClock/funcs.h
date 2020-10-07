// Function definitions

uint16_t getStrRenderLength(char *s, uint16_t limitLength=9999);
void drawStr(char *s,int16_t x, int16_t y, uint16_t limitLength=9999);
void handlePopup();
void drawHand(float x, float y, float angle, float len, uint16_t col);
void showWelcomeMessage();
void showMemoryStatus();

// Function implementations

uint16_t getStrRenderLength(char *s, uint16_t limitLength){
  uint8_t ix;
  uint16_t l=0;
  while(*s!=0 && (limitLength-->0)){
    ix=(*s)-' ';
    l+=charWidth[ix];
    ++s;
  }
  return l;
}

void drawStr(char *s,int16_t x, int16_t y, uint16_t limitLength){
  uint8_t ix;
  while(*s!=0 && (limitLength-->0)){
    ix=(*s)-' ';
    display->drawSpriteImg(&charImg,x,y,ix);
    x+=charWidth[ix];
    ++s;
  }
}

void handlePopup(){
  ++popupCnt;
  if((popupState==0 && popupCnt>=100) || (popupState==2 && popupCnt>=200)){
    ++popupState;
    popupCnt=0;
    if(popupState==1){
      popupTxtLen=0;
      if(++popupMsgIX==3)
        popupMsgIX=0;
    }
  }
  
  if(popupState==1){
    popupY-=((popupY-180.0)/5.0);
    if((popupY<220)&&(popupTxtLen<18)){
      ++popupTxtLen;
    }
    if(popupY<180.2){
      popupY=180.0;
      popupState=2;
      popupCnt=0;
    }
  }else if(popupState==2){
    if(popupTxtLen<18){
      ++popupTxtLen;
    }
  }else if(popupState==3){
    popupY+=((popupY-179)/10.0);
    if(popupTxtLen>0)
      --popupTxtLen;
    if(popupY>=240.0){
      popupY=240.0;
      popupState=0;
      popupCnt=0;
    }
  }
  if(popupState>0){
    display->setMergeAmount((240-popupY)/5);
    uint16_t l=getStrRenderLength(popupStr[popupMsgIX],popupTxtLen);
    display->setRenderType(RENDER_TYPE_MERGE);
    display->fillRect(0,popupY,240,popupY+2,(10<<11)+(20<<5)+20);
    display->fillRect(0,popupY,240,241,(10<<11)+(20<<5)+14);
    if(popupCnt<180 || (popupCnt%4)>1){
      display->setRenderPaletteIndex(1);
      drawStr(popupStr[popupMsgIX],(240-l)/2,popupY+20+(18-popupTxtLen),popupTxtLen);
      display->setRenderPaletteIndex(0);
    }
  }
}


void drawHand(float x, float y, float angle, float len, uint16_t col){
  angle+=PI;
  int16_t p0x=x+(-sin(angle)*len);
  int16_t p0y=y+(cos(angle)*len);

  int16_t p1x=x+(-sin(angle+(PI/2))*radius*handWidth);
  int16_t p1y=y+(cos(angle+(PI/2))*radius*handWidth);

  int16_t p2x=x+(-sin(angle+(PI*1.5))*radius*handWidth);
  int16_t p2y=y+(cos(angle+(PI*1.5))*radius*handWidth);

  display->fillTriangle(
    p0x,p0y,p1x,p1y,p2x,p2y,col
  );
  
}

void showWelcomeMessage(){
  // Fade display to white at boot
  for(uint16_t n=0;n<256;n+=4){
    display->clearBuffer(display->getColor16(n,n,n));
    display->drawFrame();
  }

  // Now show a welcome message (fade and scroll to centre)
  uint16_t y=20;
  uint16_t e=0;
  int16_t yDir=-1;
  while(y!=e){
    y+=yDir;
    // Fill the current renderBuffer with the specified color (off white here)
    // Note that this method always does an overwrite - it ignores the renderType setting
    display->clearBuffer(0xB5B6);
    
    // This is the slowest draw method - but allows nice effects. If the mergeAmount is set to 16 it's the same performance as the overwrite
    // render type. If the mergeAmount is set to zero, nothing is drawn.
    display->setRenderType(RENDER_TYPE_MERGE);
    display->setMergeAmount(20-y);

    drawStr("Hello",98,112-y);
    drawStr("World",95,129+y);

    // This copies the default renderBuffer to the physical display
    display->drawFrame();
    if((e==y) && (e==0)){
      delay(2000);
      yDir=1;
      e=20;
    }
  }
}

void showMemoryStatus(){
  // Just show free memory for debugging purposes
  Serial.printf("Total heap:  %d\r\n", ESP.getHeapSize());
  Serial.printf("Free heap:   %d\r\n", ESP.getFreeHeap());
  Serial.printf("Total PSRAM: %d\r\n", ESP.getPsramSize());
  Serial.printf("Free PSRAM:  %d\r\n", ESP.getFreePsram());
}
