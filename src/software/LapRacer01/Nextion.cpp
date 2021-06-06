#include <SoftwareSerial.h>
#include <Arduino.h>
#include "Nextion.h"

Nextion::Nextion(int rxPin, int txPin):_nextionSerial(rxPin, txPin){
  //
}

void Nextion::init(){
  _nextionSerial.begin(9600);
}

void Nextion::swapPage(int targetPage) {
  _nextionSerial.print("page ");
  _nextionSerial.print(targetPage);
  _nextionSerial.write(0xFF);
  _nextionSerial.write(0xFF);
  _nextionSerial.write(0xFF);
}

void Nextion::nextionWrite(String target, String value) {
  _nextionSerial.print(target);
  _nextionSerial.print("=\"");
  _nextionSerial.print(value);
  _nextionSerial.print("\"");
  _nextionSerial.write(0xFF);
  _nextionSerial.write(0xFF);
  _nextionSerial.write(0xFF);
}