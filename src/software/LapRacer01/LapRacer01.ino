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
#define BT Serial2

#define KL Serial3
#define NextionRxPin 2
#define NextionTxPin 3
SoftwareSerial Nextion(NextionRxPin, NextionTxPin);

// Timings
#define MAXSENDTIME 2000 // 2 second timeout on KDS comms.
const uint8_t ISORequestByteDelay = 10;
const uint8_t ISORequestDelay = 1000; // Time between requests.
// Source and destination adresses, ECU (0x11) & Arduino (0xF1)
const uint8_t ECUaddr = 0x11;
const uint8_t MyAddr = 0xF1;


//-----------------------------//

struct bikeData_struct {
  int rpm = 0;
  int bspeed = 0;
  int gear = 0;
  int waterTemp = 0;
  int intakeTemp = 0;
  int throttle = 0;
  bool klConnect = false;
};

bikeData_struct bikeData;

//Status LED (OnBoard)
#define BOARD_LED 13

// GPS data to send fo params
//set 10hz
byte gps10hz[] = {0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0x64, 0x00, 0x01, 0x00, 0x01, 0x00, 0x7A, 0x12};

void setup() {
  //Other serial init------------
  Serial.begin(115200);
  Serial.println("Hello LapRacer !");
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


  delay(3000);



  //questions for other things----

}

void loop() {
  ECU_requestData();
  delay(50);
}

// ##############################################
// ECU messages
// ##############################################
void ECU_sendMsg(byte message){
  for (uint8_t i = 0; i < sizeof(initMsg1)){
    
  }
}

void ECU_FastInit() {

  byte initMsg1 = {0xFE, 0x04, 0xFF, 0xFF};
  byte initMsg2 = {0x72, 0x05, 0x00, 0xF0, 0x99};
  byte respMsg;
  
  KL.end();
  // This is the ISO 14230-2 "Fast Init" sequence.
  digitalWrite(K_OUT, LOW);
  delay(70);
  digitalWrite(K_OUT, HIGH);
  delay(130);
  // Should be 10417
  KL.begin(10400);

  for (uint8_t i = 0; i < sizeof(initMsg1); i++) {
    KL.write(initMsg1[i]);
    delay(ISORequestByteDelay);
  }

  delay(20);

  for (uint8_t i = 0; i < sizeof(); i++) {
    KL.write(buf2[i]);
    delay(ISORequestByteDelay);
  }

  delay(20);

  // extracting sent message
  if (KL.available()){
    KL.readBytes();
    
  }
  // should respon: 02 04 00 FA
  while (KL.available()) {
    BT.print(KL.read(), HEX);
    BT.print(" ");
  }
  BT.println("");

  delay(20);

  BT.println("--------- Fastinit END");

}

void ECU_requestData() {

  byte reqMsg[] = {0x72, 0x05, 0x71, 0x10, 0x08};
  for (uint8_t i = 0; i < sizeof(reqMsg) ; i++) {
    KL.write(reqMsg[i]);
    delay(ISORequestByteDelay);
  }

  delay(20);

  byte respQuery[sizeof(reqMsg)];

  if (KL.available()) {
    KL.readBytes(respQuery, sizeof(reqMsg));
  }

  if (KL.available()) {
    BT.println("");
    BT.print("Reponse: ");
    while (KL.available()) {
      BT.print(KL.read(), HEX);
      BT.print(" ");
    }
  } else {
    BT.println("not connected");
    ECU_FastInit();
  }

}
