/**
cbr ECU reader
Just for tests
SoftwareSerial > pins 11/10 for bt or other
Seria0l is used for L9637D @ 10400bads 8N1

data extract:
fast init
FE 04 FF FF 
-> 72 5 00 
 <-F0 99 02 04 00 FA 

-> 72 05 71 70 A8
 <-02 08 71 70 31 80 01 63
  
-> 72 05 71 60 B8
 <-02 16 71 60 30 FD E1 9A 28 7D AF 36 93 64 FF FF 82 00 05 DD CE BE 
 
-> 72 05 71 20 F8
 <-02 08 71 20 C4 80 00 21

**/

#include <SoftwareSerial.h>


uint32_t lastBTrequest;
bool bluetoothConnected = false;

//-----------------------------//
// ##       OBD K-Line      ## //
#define K_OUT 8 // K Output Line - TX (1) on Arduino
#define K_IN 7  // K Input  Line - RX (0) on Arduino

#define GPS Serial1
//#define BT Serial2
#define BT Serial

#define KL Serial3
#define NextionRxPin 2
#define NextionTxPin 3
//SoftwareSerial Nextion(NextionRxPin, NextionTxPin);

// Timings
#define MAXSENDTIME 2000 // 2 second timeout on KDS comms.
const uint8_t ISORequestByteDelay = 10;
const uint8_t ISORequestDelay = 1000; // Time between requests.
// Source and destination adresses, ECU (0x11) & Arduino (0xF1)
const uint8_t ECUaddr = 0x11;
const uint8_t MyAddr = 0xF1;
bool ECUconnected = false;
//ToDo: Save only important parts to reduce size!!!
uint8_t ecuResponse[12];
uint32_t lastKresponse;
uint8_t ThrottlePosMax = 405;
uint8_t SubThrottleMax = 189;
//-----------------------------//

int rpm = 0;
int vitesse = 0;
int gear = 0;
int waterTemp = 0;
int intakeTemp = 0;
int throttle=0;

//test racedac format
char datatocompute[64];
char finaldata[64];
// compteur d echantillons
int countech = 0;
//compteur affichage
int countdat = 0;
int countdat1 = 90;

//Status LED (OnBoard)
#define BOARD_LED 13

//set 10hz
byte gps10hz[] = {0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0x64, 0x00, 0x01, 0x00, 0x01, 0x00, 0x7A, 0x12};

void setup() {
  //Other serial init------------

  BT.begin(115200);
  BT.println("Hello bt");
  // Nextion init ---------------
  
  // KDS Init--------------------
  pinMode(K_OUT, OUTPUT);
  pinMode(K_IN, INPUT);
  // define board status led
  pinMode(BOARD_LED, OUTPUT);
  // status led aus
  digitalWrite(BOARD_LED, LOW);
  
  lastKresponse = 0;


  delay(3000);

  

  //questions for other things----

}

void loop() {
KL.end();

  
  // This is the ISO 14230-2 "Fast Init" sequence.

  
  BT.println("--------> fast init");
/**
  uint8_t rLen;
  uint8_t req[2];
  uint8_t resp[3];
**/
  digitalWrite(K_OUT, LOW);
  delay(70);
  digitalWrite(K_OUT, HIGH);
  delay(130);
  // Should be 10417
  KL.begin(10400);

  byte buf[] = {0xFE, 0x04, 0xFF, 0xFF};
  for (uint8_t i = 0; i < 4; i++){
    KL.write(buf[i]);
    delay(ISORequestByteDelay);
  }

  delay(20);

  
  byte buf2[] = {0x72, 0x05, 0x00, 0xF0, 0x99};
  for (uint8_t i = 0; i < 5; i++){
    KL.write(buf2[i]);
    delay(ISORequestByteDelay);
  }

  delay(200);

  while(KL.available()){
    BT.print(KL.read(),HEX);
    BT.print(" ");
  }
  BT.println("");

  delay(20);
  

  

  
/**
  while(Serial1.available()){
    Serial.write(Serial1.read());
  }

  //Serial.println();

  delay(ISORequestDelay);
**/
BT.println("");



byte buf3[] = {0x72, 0x05, 0x71, 0x70, 0xA8};
  for (uint8_t i = 0; i < 5; i++){
    KL.write(buf3[i]);
    delay(ISORequestByteDelay);
  }

  delay(200);

  while(KL.available()){
    BT.print(KL.read(),HEX);
    BT.print(" ");
  }

  BT.println("");

  byte buf4[] = {0x72, 0x05, 0x71, 0x60, 0xB8};
  for (uint8_t i = 0; i < 5; i++){
    KL.write(buf4[i]);
    delay(ISORequestByteDelay);
  }

  delay(20);

  while(KL.available()){
    BT.print(KL.read(),HEX);
    BT.print(" ");
  }


  BT.println("");

  byte buf5[] = {0x72, 0x05, 0x71, 0x20, 0xF8};
  for (uint8_t i = 0; i < 5; i++){
    KL.write(buf5[i]);
    delay(ISORequestByteDelay);
  }

  delay(20);
  
  while(KL.available()){
    BT.print(KL.read(),HEX);
    BT.print(" ");
  }

  delay(1000);
  BT.println("");


}
