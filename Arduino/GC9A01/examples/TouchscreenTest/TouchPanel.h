#define MAX_FINGERS                 3

#define IT7259_ADDR                 70
#define IT7259_QUERY_STATUS_REG     (B100<<5)
#define IT7259_POINT_DATA_REG       (B111<<5)

#define GESTURE_TYPE_TAP            0x20
#define GESTURE_TYPE_PRESS          0x21
#define GESTURE_TYPE_FLICK          0x22
#define GESTURE_TYPE_DOUBLE_TAP     0x23
#define GESTURE_TYPE_TAP_AND_SLIDE  0x24
#define GESTURE_TYPE_DRAG           0x25
#define GESTURE_TYPE_DIRECTION      0x26
#define GESTURE_TYPE_TURN           0x27
#define GESTURE_TYPE_CLOCKWISE      0x28
#define GESTURE_TYPE_DIR_4WAY       0x29



#define REPORT_FORMAT_POINT_DATA    0
#define REPORT_FORMAT_WAKE_UP       1
#define REPORT_FORMAT_TOUCH_EVENT   4
#define REPORT_FORMAT_GESTURE       8

enum TouchEventType {
  TOUCH_EVENT_NONE,TOUCH_EVENT_DOWN, TOUCH_EVENT_MOVE, TOUCH_EVENT_UP
};

typedef struct FingerData {
  int16_t x,y,x2,y2;  // x2 and y2 used for flick gesture data
  int8_t pressure;
  int8_t touched;
  int8_t updated;
  TouchEventType eventType;
} FingerData;

class TouchPanel {
  
  public:

    FingerData fingerData[MAX_FINGERS];
    boolean isActive(void);
    boolean getData(void);
    TouchPanel(int16_t intPin, int16_t resetPin=-1);
    void setCallback(void (*cb)(int8_t));
    uint8_t lastGestureType;
    
  private:
    uint8_t lastCTPInt=0;
    int16_t interruptPin;
    uint8_t touchData[128]={0};

    void (*callback)(int8_t)=NULL;  
};

// Combined constructor. If reset pin is connected to reset pin for main TFT controller, then no need to pass it in.
TouchPanel::TouchPanel(int16_t intPin,int16_t rstPin){
  if(rstPin>-1){
    // Must reset the cap touch controller before use! Could probably link this to the same reset pin used for the main display.
    pinMode(rstPin,OUTPUT);
    digitalWrite(rstPin,LOW);
    delay(500);
    digitalWrite(rstPin,HIGH);
    delay(500); // Need a slight delay after reset, so might as well do it here so the controller is ready after creating the instance
  }
  interruptPin=intPin;

    // Capacitive touch
  pinMode(interruptPin,INPUT_PULLUP);

  for(uint16_t n=0;n<MAX_FINGERS;n++){
    fingerData[n].touched=0;
    fingerData[n].updated=0;
    fingerData[n].eventType=TOUCH_EVENT_NONE;
  }
}

boolean TouchPanel::isActive(void){
  Wire.beginTransmission(IT7259_ADDR);
  if(Wire.endTransmission()!=0)
    return false;
  return true;
}

boolean TouchPanel::getData(void){

 if(digitalRead(interruptPin)==LOW){
    if(!lastCTPInt){
      lastCTPInt=1;
    }else{
      return false;
    }
  }else{
    lastCTPInt=0;
    return false;
  }

  boolean ret=false;
  int8_t reportFormat=-1;
  Wire.beginTransmission(IT7259_ADDR);
  Wire.write(IT7259_QUERY_STATUS_REG);
  Wire.endTransmission();
  delay(1); // Give it a little time to respond...
  uint16_t readBytes=Wire.requestFrom(IT7259_ADDR, 1);
  if(readBytes>0){
    uint8_t queryData=Wire.read();
    if(queryData&(B11<<6)){

      // Get point data!
      Wire.beginTransmission(IT7259_ADDR);
      Wire.write(IT7259_POINT_DATA_REG);
      Wire.endTransmission();
      readBytes=Wire.requestFrom(IT7259_ADDR, 13);
      if(readBytes==13){
        for(uint16_t n=0;n<readBytes;n++){
          touchData[n]=Wire.read();
        }
        
        reportFormat=touchData[0]>>4;
        switch(reportFormat){
          
          case REPORT_FORMAT_POINT_DATA: // Raw point data
          {
            if((touchData[0]&(1<<3))==0){
              for(uint16_t n=0;n<MAX_FINGERS;n++){
                if(fingerData[n].touched){
                  fingerData[n].eventType=TOUCH_EVENT_UP;
                }else{
                  fingerData[n].eventType=TOUCH_EVENT_NONE;
                }
                fingerData[n].touched=0;
              }
            }else{
              uint16_t p;
              const uint16_t baseIX=2;
              for(uint16_t n=0;n<MAX_FINGERS;n++){
                fingerData[n].updated=0;
                if((touchData[0]&(1<<n))){  // Pointdata available for finger X
                  if(!fingerData[n].touched){
                    fingerData[n].eventType=TOUCH_EVENT_DOWN;
                  }else{
                    fingerData[n].eventType=TOUCH_EVENT_MOVE;
                  }
                  fingerData[n].updated=1;
                  fingerData[n].touched=1;
                  p=((touchData[baseIX+1+(n*4)] & 0x0f)<<8)+(touchData[baseIX+0+(n*4)]);
                  fingerData[n].x=p;
                  p=((touchData[baseIX+1+(n*4)] & 0xf0)<<4)+(touchData[baseIX+2+(n*4)]);
                  fingerData[n].y=p;
                  fingerData[n].pressure=(touchData[5+(n*4)] & 0x0f);
                }else if(fingerData[n].touched){
                  fingerData[n].eventType=TOUCH_EVENT_UP;
                  fingerData[n].touched=0;
                }
              }
            }
          }
          ret=true;
          break;

          case REPORT_FORMAT_GESTURE:
            lastGestureType=touchData[1];
            switch(lastGestureType){
              
              case GESTURE_TYPE_TAP:
              case GESTURE_TYPE_PRESS:
              case GESTURE_TYPE_DOUBLE_TAP:
                fingerData[0].x=touchData[2]+(touchData[3]<<8);
                fingerData[0].y=touchData[4]+(touchData[5]<<8);
                break;

              case GESTURE_TYPE_FLICK:
                fingerData[0].x=touchData[2]+(touchData[3]<<8);
                fingerData[0].y=touchData[4]+(touchData[5]<<8);
                fingerData[0].x2=touchData[6]+(touchData[7]<<8);
                fingerData[0].y2=touchData[8]+(touchData[9]<<8);
                break;

              default:  // Ignore other gestures
                break;
            }
            ret=true;
          break;

          default:  // Unknown/unprocessed event
            reportFormat=-1;
        } // Switch
      }
    }
  }
  
  if(ret && callback!=NULL){
    callback(reportFormat);
  }
  
  return ret;
}

void TouchPanel::setCallback(void (*cb)(int8_t)){
  callback=cb;
}
