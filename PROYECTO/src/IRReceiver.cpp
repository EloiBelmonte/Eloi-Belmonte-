#include "IRReceiver.h"

IRReceiver::IRReceiver(int pin) : pin_(pin), irrecv_(pin) {}

void IRReceiver::begin() {
    irrecv_.enableIRIn();
}

unsigned long IRReceiver::getIRCode() {
    if (irrecv_.decode(&results_)) {
        unsigned long code = results_.value;
        irrecv_.resume();
        return code;
    }
    return 0;
}