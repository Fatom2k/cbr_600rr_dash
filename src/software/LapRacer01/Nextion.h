/*
  Morse.h - Library for flashing Morse code.
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.
*/


#ifndef Nextion_h
#define Nextion_h

#include <Arduino.h>
#include <SoftwareSerial.h>

class Nextion
{

  public:
    Nextion(int rxPin, int txPin);
    void init();
    void swapPage(int targetPage);
    void nextionWrite(String target, String value);

  protected:
    SoftwareSerial _nextionSerial;
    

};

#endif

