

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
    void setRpm(int rpm);
    void setSpeed(int speed);
    void setTemp(int temp_ldr);


  protected:
    SoftwareSerial _nextionSerial;
    int _rpm;
    int _rengage;
    int _vitesse;
    int _temp_ldr;
    int _essence;
    
    void _nextionWrite(String target, String value);
    void _nextionWriteText(String target, String value);
    

};

#endif

