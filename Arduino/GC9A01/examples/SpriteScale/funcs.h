// Function definitions

uint16_t getStrRenderLength(const char *s, uint16_t limitLength=9999);
int16_t drawStr(const char *s,int16_t x, int16_t y, uint16_t limitLength=9999);
int16_t drawCentredStr(const char *s, int16_t y, uint16_t limitLength=9999);
void drawAnimatedCentredStr(const char *s, int16_t y, uint16_t limitLength, float mag);
void updateTextWriter(void);
void showWelcomeMessage();
void showMemoryStatus();
void doGhostAnim(uint16_t op);

// Function implementations

uint16_t getStrRenderLength(const char *s, uint16_t limitLength){
  uint16_t l=0;
  while(*s!=0 && (limitLength-->0)){
    l+=charWidth[(*s++)-' ']+1;
  }
  return l;
}

int16_t drawStr(const char *s,int16_t x, int16_t y, uint16_t limitLength){
  uint8_t ix;
  while(*s!=0 && (limitLength-->0)){
    ix=(*s++)-' ';
    display->drawSpriteImg(&charGfx,x,y,ix);
    x+=charWidth[ix]+1;
  }
  return x;
}

int16_t drawCentredStr(const char *s, int16_t y, uint16_t limitLength){
  uint16_t s0X=(240-getStrRenderLength(s,limitLength))/2;
  return drawStr(s,s0X,y,limitLength);
}

void drawAnimatedCentredStr(const char *s, int16_t y, uint16_t limitLength, float mag){
  display->setRenderType(RENDER_TYPE_OVERWRITE);
  int16_t txtEnd=drawCentredStr(s, y, limitLength);
  int16_t ix=s[limitLength]-' ';
  if(ix>=0){
    float w=(1.0+(mag*20))*charWidth[ix];
    float h=(1.0+(mag*20))*8;
    display->setRenderType(RENDER_TYPE_MERGE);
    display->setMergeAmount(16-(mag*15));
    display->drawSpriteImg(&charGfx,txtEnd+(mag*10)-(w/2),(y-(80*mag))-(h/2),w,h,ix);
  }
}

void updateTextWriter(void){
  stringSubPos-=0.15;
  if(stringSubPos<=0){
    stringSubPos=1;
    if(writerTxt[stringIX][stringPos]!=0){
      ++stringPos;
    }else{
      if(writerPause>0){
        --writerPause;
      }else{
        writerPause=10;
        if(++stringIX==7){
          stringIX=1;
        }
        stringPos=0;
        stringSubPos=1;
      }
    }
  }
  if(wop<10)
    ++wop;
  display->setMergeAmount(wop);
  display->fillRect(0,193,240,212,(8<<11)+(16<<5));
  drawAnimatedCentredStr(writerTxt[stringIX],200,stringPos,stringSubPos);
}

void showWelcomeMessage(){
    uint16_t op=0;
    uint16_t rop=0;
    for(uint16_t n=0;n<300;n++){
      display->clearBuffer(0);
      if((n<140) && (op<16) && ((n%5)==0)){
        ++op;
      }else if((n>210) && (op>0) && ((n%5)==0)){
        --op;
      }
      if((n<140) && (rop<10) && ((n%5)==0)){
        ++rop;
      }else if((n>210) && (rop>0) && ((n%5)==0)){
        --rop;
      }
      doGhostAnim(op);
      display->setMergeAmount(rop);
      display->fillRect(0,193,240,212,(8<<11)+(16<<5));
      display->setRenderType(RENDER_TYPE_OVERWRITE);
      int16_t lim=0;
      if(n<200){
        lim=n/4;
      }else{
        lim=(299-n)/4;
      }
      drawCentredStr(writerTxt[0],200,lim);
      display->drawFrame();
    }
}

void showMemoryStatus(){
  // Just show free memory for debugging purposes
  Serial.printf("Total heap:  %d\r\n", ESP.getHeapSize());
  Serial.printf("Free heap:   %d\r\n", ESP.getFreeHeap());
  Serial.printf("Total PSRAM: %d\r\n", ESP.getPsramSize());
  Serial.printf("Free PSRAM:  %d\r\n", ESP.getFreePsram());
}

void doGhostAnim(uint16_t op){
  
  display->setRenderType(RENDER_TYPE_MERGE);
  display->setMergeAmount(op);
  display->drawSpriteImg(&ghostGfx,120-(w/2),120-(h/2),w,h,0,0);
    
  w+=wDir;
  if(w>=420)
    wDir=-spd;
  else if(w<=4)
    wDir=spd;

  h+=hDir;
  if(h>=280)
    hDir=-spd;
  else if(h<=4)
    hDir=spd;

}
