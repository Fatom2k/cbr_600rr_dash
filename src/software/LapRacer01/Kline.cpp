#include <Arduino.h>
#include <stdio.h>
#include "KLine.h"


// K-Line delay
const uint8_t ISORequestByteDelay = 10;

// Ecu connected
bool ECU_connected = false;

// K-Line Honda messages
byte initMsg1[] = {0xFE, 0x04, 0xFF, 0xFF};
byte initMsg2[] = {0x72, 0x05, 0x00, 0xF0, 0x99};
byte respInitMsg2[] = {0x02, 0x04, 0x00, 0xFA};
byte reqTable10[] = {0x72, 0x05, 0x71, 0x10, 0x08};


void ECU_init() {
  pinMode(K_OUT, OUTPUT);
  pinMode(K_IN, INPUT);
}

// mandatory

void ECU_printMsg(byte *msgToPrint, int msgSize) {

  for (int i = 0; i < msgSize; i++) {
    Serial.print(msgToPrint[i], HEX);
    Serial.print(" ");
  }
  Serial.println("");
}

// Send data to ECU through K-Line
void ECU_sendMsg(byte *message, int msgSize) {
  Serial.print("--> Sending message: ");
  ECU_printMsg(message, message[1]);
  for (uint8_t i = 0; i < msgSize; i++) {
    KL.write(message[i]);
    delay(ISORequestByteDelay);
  }
  delay(20);
}

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

// Ask for data and verifying answer
byte ECU_requestData(byte *reqMsg) {
  byte respMsg[32];
  for (uint8_t j = 0; j < 32; j++) {
    respMsg[j] = 0x00;
  }

  uint8_t reqMsgSize = reqMsg[1];

  if (!ECU_connected) {
    ECU_fastInit();
    return 1;

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

    Serial.print("<-- Answer: ");
    ECU_printMsg(respMsg, sizeof(respMsg));

    // parsing answer
    // exemple: -> 72 05 71 10 08 <- 02 16 71 10 00 00 1C 00 97 41 94 42 93 64 FF FF 82 00 00 00 80 A6
    // TODO: extraction de la reqMsg  qui est l'echo
    // TODO: comparer les octets 3 et 4 de la question et la réponse qui doivent être égaux
    // Dans notre cas les octets @2 et @3 et + @1

    if ( ( respMsg[0 + reqMsgSize] ==  0x02 ) and (respMsg[2] == respMsg[2 + reqMsgSize]) and (respMsg[3] == respMsg[3 + reqMsgSize]) ) {
      Serial.println("ECU respond something");
      //return 1;
    } else {
      Serial.println("ECU doesn't respond");
      ECU_connected = false;
    }
  }

  //return 0;

}

// Initiate ECU  Fastinit
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
  delay(200);
  //FastInit 2nd message. Should have response if ECU Connected
  ECU_connected = ECU_sendFastInitMsg(initMsg2);
  delay(200);
  

  //TODO: test message length and checksum
}

void ECU_updateDatas() {
  ECU_requestData(reqTable10);
}
