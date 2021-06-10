/**
  cbr ECU reader
  Just for tests
  SoftwareSerial > pins 11/10 for bt or other
  Seria0l is used for L9637D @ 10400bads 8N1

**/

#include "Nextion.h"
#include "KLine.h"

/**
    #######################
    Hardware definitions
    #######################
**/

#define GPS Serial1
#define BT Serial2
//#define KL Serial3

// KLine peripherals. Mapped on Serial3.
//#define K_OUT 8 // K Output Line 
//#define K_IN 7  // K Input  Line 
//#define KL Serial3
//KLine kline(K_IN, K_OUT);

#define NextionRxPin 5
#define NextionTxPin 6
Nextion nextion(NextionRxPin, NextionTxPin);


//Status LED (OnBoard)
#define BOARD_LED 13



// GPS data to send fo params
//set 10hz
byte gps10hz[] = {0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0x64, 0x00, 0x01, 0x00, 0x01, 0x00, 0x7A, 0x12};


/**
    #######################
    Setup is here
    #######################
**/
void setup() {
  //Other serial init------------
  Serial.begin(115200);
  BT.begin(115200);

  // Nextion init ---------------
  nextion.init();

  // KDS Init--------------------
  ECU_init();
  
  // define board status led
  pinMode(BOARD_LED, OUTPUT);
  // status led aus
  digitalWrite(BOARD_LED, LOW);


  //questions for other things----


}

/**
    #######################
    Program main loop
    #######################
**/
void loop() {
  //ECU_requestData();
  //nextion.nextionSwapPage(0);
  ECU_updateDatas();
  delay(100);

}

/**
    #######################
    Functions for Honda K-Line ECU
    #######################
**/







