/**
  cbr ECU reader
  Just for tests
  SoftwareSerial > pins 11/10 for bt or other
  Seria0l is used for L9637D @ 10400bads 8N1

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

/**
Send data to ECU through K-Line
**/
void ECU_sendMsg(byte message){
  for (uint8_t i = 0; i < sizeof(initMsg1)){
    KL.write(message[i]);
    delay(ISORequestByteDelay);    
  }
  delay(20);
}

/**
Initiate ECU  Fastinit
**/
void ECU_FastInit() {

  byte initMsg1 = {0xFE, 0x04, 0xFF, 0xFF};
  byte initMsg2 = {0x72, 0x05, 0x00, 0xF0, 0x99};
  byte respQuery;
  byte respMsg;
  
  Serial.println("> FastInit");

  KL.end();
  // This is the ISO 14230-2 "Fast Init" sequence.

  // FastInit line
  digitalWrite(K_OUT, LOW);
  delay(70);
  digitalWrite(K_OUT, HIGH);
  delay(130);
  // Should be 10417
  KL.begin(10400);

  // FastInit 1st message
  ECU_sendMsg(initMsg1);

  //FastInit 2nd message. Should have response if ECU Connected
  ECU_sendMsg(initMsg2);
  // extracting sent message because it echoes itself
  if (KL.available()){
    KL.readBytes(respQuery, sizeof(initMsg2));  
  }

  // Extracting Response. Should respond: 02 04 00 FA
  if (KL.available()){
      KL.readBytes(respMsg, 4); //we have a fixed length answer because we are waiting for a specified msg
      bikeData.klConnect = true;
      Serial.println("< II: ECU connectec");
  } else {
    bikeData.klConnect = false;
    Serial.println("< EE: ECU not connected");
  }

  //TODO: test message length and checksum
  
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
