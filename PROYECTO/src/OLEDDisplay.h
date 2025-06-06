#ifndef OLED_DISPLAY_H
#define OLED_DISPLAY_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class OLEDDisplay {
public:
    OLEDDisplay(int width, int height, int resetPin);
    bool begin();
    void showMenu(int menuSelection);
    void showUploadNotification();
    void updateDisplay(const String& track, float volume, bool paused);
    void showText(const String& text);
    void clear();

private:
    int width_;
    int height_;
    int resetPin_;
    Adafruit_SSD1306 display_;
    String lastDisplayedTrack_;
    float lastDisplayedVolume_;
    bool lastDisplayedPaused_;
    String lastDisplayedText_; // New: Store the last displayed text
};

#endif