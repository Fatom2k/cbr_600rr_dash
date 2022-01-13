#include <SoftwareSerial.h>
#include <Arduino.h>
#include "Nextion.h"

Nextion::Nextion(int rxPin, int txPin):_nextionSerial(rxPin, txPin){
  //
}

void Nextion::init(){
  _nextionSerial.begin(9600);
  delay(1000);
  swapPage(1);
  delay(3000);
  swapPage(0);
  delay(1500);
  setRpm(0);
  setSpeed(0);
  //nextionWrite("temp_ldr.txt", "0");

}

// specific screen updates
void Nextion::setRpm(int rpm){
  _nextionWrite("rpm.txt", String (rpm));
}

void Nextion::setSpeed(int speed){
  _nextionWrite("vitesse.txt", String(speed));
}

void Nextion::setGear(char gear){
  _nextionWrite("rengage.txt", gear);
}

void Nextion::setECT(int temp){
  _nextionWrite("temp_ldr.txt", String(temp));
}

// mandatory
void Nextion::_nextionWrite(String target, String value) {
  _nextionSerial.print(target);
  _nextionSerial.print("=\"");
  _nextionSerial.print(value);
  _nextionSerial.print("\"");
  _nextionSerial.write(0xFF);
  _nextionSerial.write(0xFF);
  _nextionSerial.write(0xFF);
}

void Nextion::_nextionWriteText(String target, String value) {
  _nextionSerial.print(target);
  _nextionSerial.print("=\"");
  _nextionSerial.print(value);
  _nextionSerial.print("\"");
  _nextionSerial.write(0xFF);
  _nextionSerial.write(0xFF);
  _nextionSerial.write(0xFF);
}

void Nextion::swapPage(int targetPage) {
  _nextionSerial.print("page ");
  _nextionSerial.print(targetPage);
  _nextionSerial.write(0xFF);
  _nextionSerial.write(0xFF);
  _nextionSerial.write(0xFF);
}
