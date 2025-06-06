#ifndef BUTTON_MANAGER_H
#define BUTTON_MANAGER_H

#include <Arduino.h>

class ButtonManager {
public:
    ButtonManager(int volUpPin, int volDownPin, int pausePin, int restartPin, int skipPin);
    void begin();
    bool isVolumeUpPressed();
    bool isVolumeDownPressed();
    bool isPausePressed();
    bool isRestartPressed();
    bool isSkipPressed();

private:
    int volUpPin_;
    int volDownPin_;
    int pausePin_;
    int restartPin_;
    int skipPin_;
    unsigned long lastButtonPress_;
    const unsigned long debounceDelay_ = 100;
    bool checkButton(int pin);
};

#endif