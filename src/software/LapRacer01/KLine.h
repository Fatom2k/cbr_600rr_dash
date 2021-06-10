#ifndef KLine_h
#define KLine_h

#include <Arduino.h>

#define K_OUT 8 // K Output Line 
#define K_IN 7  // K Input  Line 
#define KL Serial3

void ECU_init();
void ECU_fastInit();
void ECU_updateDatas();


#endif