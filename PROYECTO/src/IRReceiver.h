#ifndef IR_RECEIVER_H
#define IR_RECEIVER_H

#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

class IRReceiver {
public:
    IRReceiver(int pin);
    void begin();
    unsigned long getIRCode();

private:
    int pin_;
    IRrecv irrecv_;
    decode_results results_;
};

#endif