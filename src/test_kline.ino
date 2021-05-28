/**
cbr ECU reader
Just for tests
SoftwareSerial > pins 11/10 for bt or other
Serial is used for L9637D @ 10400bads 8N1

**/

//#include <SoftwareSerial.h>

//-----------------------------//
// ##    Bluetooth HC-06    ## //
//Serial com. on Digital Pin 10 Rx & 11 Tx
// SoftwareSerial BT(11, 10);
uint32_t lastBTrequest;
bool bluetoothConnected = false;

//-----------------------------//
// ##       OBD K-Line      ## //
#define K_OUT 1 // K Output Line - TX (1) on Arduino
#define K_IN 0  // K Input  Line - RX (0) on Arduino

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

  Serial.begin(9600);
  Serial.println("Hello bt");
  // KDS Init--------------------
  pinMode(K_OUT, OUTPUT);
  pinMode(K_IN, INPUT);
  // define board status led
  pinMode(BOARD_LED, OUTPUT);
  // status led aus
  digitalWrite(BOARD_LED, LOW);
  
  lastKresponse = 0;
  

  //questions for other things----

}

void loop() {

  Serial.println("--------> debut loop");

  uint8_t rLen;
  uint8_t req[2];
  uint8_t resp[3];

  Serial1.end();

  
  // This is the ISO 14230-2 "Fast Init" sequence.

  digitalWrite(K_OUT, LOW);
  delay(70);
  digitalWrite(K_OUT, HIGH);
  delay(130);
  // Should be 10417
  Serial1.begin(10400);

  byte buf[] = {0xFE, 0x04, 0xFF, 0xFF};
  for (uint8_t i = 0; i < 4; i++){
    Serial1.write(buf[i]);
    delay(ISORequestByteDelay);
  }

  delay(50);

  
/**
  while(Serial1.available()){
    Serial.write(Serial1.read());
  }

  //Serial.println();

  delay(ISORequestDelay);
**/


  byte buf2[] = {0x72, 0x05, 0x00, 0xF0, 0x99};
  for (uint8_t i = 0; i < 5; i++){
    Serial1.write(buf2[i]);
    delay(ISORequestByteDelay);
  }



  delay(20);

  //Serial.println();

  while(Serial1.available()){
    Serial.write(Serial1.read());
  }

  delay(1000);

  Serial.println("---------> Fin loop");


}
