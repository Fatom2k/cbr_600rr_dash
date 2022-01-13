#include <Arduino.h>
#include <stdio.h>
#include "KLine.h"


// requestData struc answer
struct requestDataAnswerStruct {
  bool result = false;
  byte answer[32] = {0, 0, 0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
};

// datas stored from bike 
struct bikeDataStruct {
    int rpm = 0;
    int speed = 0;
    int gear = 0;
    int coolant_temp = 0;
};

// Ecu connected
bool ECU_connected = false;

// K-Line delay
const uint8_t ISORequestByteDelay = 10;

// K-Line Honda messages
byte initMsg1[] = {0xFE, 0x04, 0xFF, 0xFF};
byte initMsg2[] = {0x72, 0x05, 0x00, 0xF0, 0x99};
byte respInitMsg2[] = {0x02, 0x04, 0x00, 0xFA};
byte reqTable10[] = {0x72, 0x05, 0x71, 0x10, 0x08}; // tables principale

// usefull instances of structs


bikeDataStruct bikeData;
bikeDataStruct previousBikeData;

void ECU_init() {
  pinMode(K_OUT, OUTPUT);
  pinMode(K_IN, INPUT);
}

// mandatory

// just a debug printing of ECU messages into serial ----------------------------------------------
void ECU_printMsg(byte *msgToPrint, int msgSize) {

  for (int i = 0; i < msgSize; i++) {
    Serial.print(msgToPrint[i], HEX);
    Serial.print(" ");
  }
  Serial.println("");
}

// Send data to ECU through K-Line ----------------------------------------------------------------
void ECU_sendMsg(byte *message, int msgSize) {
  Serial.print("--> Sending message: ");
  ECU_printMsg(message, message[1]);
  for (uint8_t i = 0; i < msgSize; i++) {
    KL.write(message[i]);
    delay(ISORequestByteDelay);
  }
  delay(20);
}

// send fastInit messages because they are specific------------- ----------------------------------
bool ECU_sendFastInitMsg(byte *reqMsg) {
  byte respMsg[5];
  for (uint8_t j = 0; j < 5; j++) {
    respMsg[j] = 0x00;
  }

  uint8_t reqMsgSize = reqMsg[1];
  // sending message
  ECU_sendMsg(reqMsg, reqMsgSize);

  // reading answer
  int i = 0;
  while (KL.available()) {
    respMsg[i] = KL.read();
    i++;
  }

  Serial.print("<-- Answer: ");
  ECU_printMsg(respMsg, sizeof(respMsg));

  // on teste si le message est fastinit 1
  if ( (respMsg[0] == initMsg1[0]) and (respMsg[1] == initMsg1[1]) and (respMsg[2] == initMsg1[2]) and (respMsg[3] == initMsg1[3]) ) {
    Serial.println(" 1st Fastinit message: No Answer needed!");
    return 0;
  }
  // on teste si le message est fastinit 2
  if ((respMsg[0] == initMsg2[0]) and (respMsg[1] == initMsg2[1]) and (respMsg[2] == initMsg2[2]) and (respMsg[3] == initMsg2[3]) and (respMsg[4] == initMsg2[4])) {
    Serial.print(" 2nd FastInit message: ");
    if ( ( respMsg[0 + reqMsgSize] ==  respInitMsg2[0] ) and (respMsg[1 + reqMsgSize] == respInitMsg2[1] ) and (respMsg[2 + reqMsgSize] == respInitMsg2[2]) and (respMsg[3 + reqMsgSize] == respInitMsg2[3])) {
      Serial.println("ECU is up dude !!");
      delay(20); // allowing delay before sending next payload
      return true;
    } else {
      Serial.println("ECU doesn't respond");
      return false;
    }
  }

  return false;

}

// Ask for data and verifying answer --------------------------------------------------------------
// TODO: format return with the struct
// TODO: test checkSum
requestDataAnswerStruct ECU_requestData(byte *reqMsg) {
  requestDataAnswerStruct requestDataAnswer;
  byte respMsg[32];
  for (uint8_t j = 0; j < 32; j++) {
    respMsg[j] = 0x00;
  }

  uint8_t reqMsgSize = reqMsg[1];

  if (!ECU_connected) {
    ECU_fastInit();

    return requestDataAnswer; // if ecu was not connected, fast init apended so we must return or it freezes
  }

  if (ECU_connected) {
    // sending message
    ECU_sendMsg(reqMsg, reqMsgSize);
    // reading answer
    int i = 0;
    while (KL.available()) {
      respMsg[i] = KL.read();
      i++;
    }

    //Serial.print("<-- Answer: ");
    //ECU_printMsg(respMsg, sizeof(respMsg));

    // parsing answer
    // exemple: -> 72 05 71 10 08 <- 02 16 71 10 00 00 1C 00 97 41 94 42 93 64 FF FF 82 00 00 00 80 A6
    // Dans notre cas les octets @2 et @3 et + @1

    if ( ( respMsg[0 + reqMsgSize] ==  0x02 ) and (respMsg[2] == respMsg[2 + reqMsgSize]) and (respMsg[3] == respMsg[3 + reqMsgSize]) ) {
      Serial.println("ECU respond something");
      requestDataAnswer.result = true;
      for (int i = 0; i<32; i++){
        requestDataAnswer.answer[i] = respMsg[i + reqMsgSize];
      };
      return requestDataAnswer;
      
    } else {
      Serial.println("ECU doesn't respond");
      ECU_connected = false;
      return requestDataAnswer;
    }
  }
  return requestDataAnswer;
}

// Initiate ECU  Fastinit -------------------------------------------------------------------------
void ECU_fastInit() {

  Serial.println("--> Starting Fastinit");
  byte *respQuery;
  byte *respMsg;

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
  ECU_sendFastInitMsg(initMsg1);
  delay(200); //mandatory delay 
  //FastInit 2nd message. Should have response if ECU Connected
  ECU_connected = ECU_sendFastInitMsg(initMsg2);
  delay(100); // allow delay after fastInit procedure
}

// Update bike datas ------------------------------------------------------------------------------
void ECU_updateDatas() {
  requestDataAnswerStruct requestDataAnswer;

  //requesting main data table
  requestDataAnswer = ECU_requestData(reqTable10);
  if (requestDataAnswer.result) {
    // if there s an answer, parser result
    //ECU_printMsg(requestDataAnswer.answer, 32);
    // example 02 16 71 10 00 00 1C 00 97 41 94 42 93 64 FF FF 82 00 00 00 80 A6

    // rpm registers @4 and @5
    bikeData.rpm = 0;
    bikeData.rpm = requestDataAnswer.answer[4]; //msb. dont forget to add the query offset
    bikeData.rpm = (bikeData.rpm << 8) | requestDataAnswer.answer[5]; //lsb dont forget to add the query offset
    Serial.print("=======RPM: ");
    Serial.println(bikeData.rpm);

    // speed register @17
    bikeData.speed = requestDataAnswer.answer[17];
    Serial.print("=======Speed: ");
    Serial.println(bikeData.speed);

    // engaged gear (compute from rpm and speed)
    bikeData.gear = 0;

    // coolant temperature @9
    bikeData.coolant_temp = requestDataAnswer.answer[9] - 40;
    Serial.print("=======ECT: ");
    Serial.println(bikeData.coolant_temp);

  }
  
}

void ECU_getDatas(int datas[4]) {
  ECU_updateDatas();
  datas[0]=bikeData.rpm;
  datas[1]=bikeData.speed;
  datas[2]=bikeData.gear;
  datas[3]=bikeData.coolant_temp;
}
