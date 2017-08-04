
#include <tactigon_led.h>
#include <tactigon_IMU.h>
#include <tactigon_BLE.h>
#include "IMU_main.h"

T_Led rLed, bLed, gLed;

T_QUAT qMeter;
T_QData qData;

T_BLE bleManager;
UUID uuid;
T_BLE_Characteristic bleChar;

int ticks;
unsigned short cnt;
unsigned char buffData[8];

extern float roll, pitch, yaw;
extern uint8_t checkPattern_VertSwipe(float roll_v, float pitch_v, float yaw_v);
extern uint8_t checkPattern_Tap(float roll_t, float pitch_t, float yaw_t);
extern uint8_t checkPattern_OrizSwipe(float roll_o, float pitch_o, float yaw_o);
extern void eulerAngles(float q0, float q1, float q2, float q3);


void setup() {
  
  ticks = 0;
  cnt = 0;
  rLed.init(T_Led::RED);
  gLed.init(T_Led::GREEN);
  bLed.init(T_Led::BLUE);

  rLed.off();
  gLed.off();
  bLed.off();

  //add acc characteristic
  uuid.set("7ac71000-503d-4920-b000-9e57000000001");  
  bleChar = bleManager.addNewChar(uuid, 8);
}

void loop() { 
  
  unsigned short unused_16;
  unsigned int unused_32;
  unsigned short gestNotification;

  //init buffer used to stream on ble characteristic
  memset(buffData, 0, 8);


  //update BLE characteristic @ 50Hz
  if(GetCurrentMilli() >= (ticks +(1000 / 50))){
    
    ticks = GetCurrentMilli();
    cnt++;
    memcpy(&buffData[0], &cnt, 2);

    //read sensors
    IMU_loop();
              
    //get quaternions data
    qData = qMeter.getQs();                   

    //compute Euler angles
    eulerAngles(qData.q0f, qData.q1f, qData.q2f, qData.q3f);

    //check for Vertical swipe
    if(checkPattern_VertSwipe(roll, pitch, yaw) == 1){
      
      gestNotification = 1;
      
      //prepare buffer to stream on ble characteristic (first 6 byte are reserved for future uses)     
      memcpy(&buffData[6], &gestNotification, 2);
  
      //update ble characteristic
      bleChar.update(buffData);
    }

    //check for Orizontal swipe
    if(checkPattern_OrizSwipe(roll, pitch, yaw) == 1){
      
      gestNotification = 2;
      
      //prepare buffer to stream on ble characteristic (first 6 byte are reserved for future uses)     
      memcpy(&buffData[6], &gestNotification, 2);
  
      //update ble characteristic
      bleChar.update(buffData);
    }

    //check for Tap/click
    if(checkPattern_Tap(roll, pitch, yaw)){
      
      gestNotification = 3;
      
      //prepare buffer to stream on ble characteristic (first 6 byte are reserved for future uses)     
      memcpy(&buffData[6], &gestNotification, 2);
  
      //update ble characteristic
      bleChar.update(buffData);
    }
    
  }
}




