// This is currently just test code! Not connected to the main library in anyway...

#include <Arduino.h>
#include <Wire.h>

#include "TouchPanel.h"

// Default i2C pins on ESP32
// SCL = 22
// SDA = 21

// GPIO connected to touch panel interrupt pin
#define CTP_INT                 32

// GPIO connected to touch panel reset pin
#define CTP_RST                 16

TouchPanel *touchPanel;

// This gets called whenever there is a touch event. The TouchPanel instance must then be used to work out what happened
void testCallback(int8_t formatType){
  Serial.printf("Called with type: %i\r\n",formatType);
  switch(formatType){
    case REPORT_FORMAT_POINT_DATA:
    {
      FingerData *f;
      for(uint16_t n=0;n<MAX_FINGERS;n++){
        f=&touchPanel->fingerData[n];
        if(f->updated){
          Serial.printf("Finger[%i]: x=%i, y=%i, pressure=%i, eventType=%i\r\n",n,f->x,f->y,f->pressure,(int)f->eventType);
        }
      }
    }
    break;

    case REPORT_FORMAT_GESTURE:
    {
      // Just report code for gesture type for testing. Table of gesture types in TouchPanel.h file
      Serial.printf("Gesture type: %i\r\n",touchPanel->lastGestureType);
    }
    break;
  }
}

void setup(){
  Serial.begin(115200);
  Serial.println("Startup...");
  // Need to enable the i2C peripheral before we access the TouchPanel
  Wire.setClock(100000);
  Wire.begin(); // i2c used for both the temp/humidity sensor, and the capacitive touch input controller
  touchPanel=new TouchPanel(CTP_INT,CTP_RST);
  if(touchPanel->isActive()){
    touchPanel->setCallback(testCallback);  // Callback to accept touch event notifications
    Serial.println("Touch controller ok");
  }else{
    touchPanel=NULL;
    Serial.println("Touch controller missing!");
  }
}

void loop(){
  if(touchPanel!=NULL){
    touchPanel->getData();
  }
  delay(1);
}
