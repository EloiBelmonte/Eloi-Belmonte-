#include "ButtonManager.h"

ButtonManager::ButtonManager(int volUpPin, int volDownPin, int pausePin, int restartPin, int skipPin)
    : volUpPin_(volUpPin), volDownPin_(volDownPin), pausePin_(pausePin), restartPin_(restartPin), skipPin_(skipPin), lastButtonPress_(0) {}

void ButtonManager::begin() {
    pinMode(volUpPin_, INPUT_PULLUP);
    pinMode(volDownPin_, INPUT_PULLUP);
    pinMode(pausePin_, INPUT_PULLUP);
    pinMode(restartPin_, INPUT_PULLUP);
    pinMode(skipPin_, INPUT_PULLUP);
}

bool ButtonManager::checkButton(int pin) {
    unsigned long currentTime = millis();
    if (digitalRead(pin) == LOW && (currentTime - lastButtonPress_) > debounceDelay_) {
        lastButtonPress_ = currentTime;
        return true;
    }
    return false;
}

bool ButtonManager::isVolumeUpPressed() {
    return checkButton(volUpPin_);
}

bool ButtonManager::isVolumeDownPressed() {
    return checkButton(volDownPin_);
}

bool ButtonManager::isPausePressed() {
    return checkButton(pausePin_);
}

bool ButtonManager::isRestartPressed() {
    return checkButton(restartPin_);
}

bool ButtonManager::isSkipPressed() {
    return checkButton(skipPin_);
}