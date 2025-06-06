#include "OLEDDisplay.h"
#include <Wire.h>

OLEDDisplay::OLEDDisplay(int width, int height, int resetPin)
    : width_(width), height_(height), resetPin_(resetPin), display_(width, height, &Wire, resetPin) {
    lastDisplayedVolume_ = -1;
    lastDisplayedPaused_ = false;
    lastDisplayedText_ = ""; // Initialize empty
}

bool OLEDDisplay::begin() {
    if (!display_.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        return false;
    }
    display_.setTextSize(1);
    display_.setTextColor(SSD1306_WHITE);
    display_.clearDisplay();
    display_.setCursor(0, 0);
    display_.println("Iniciando...");
    display_.display();
    lastDisplayedText_ = "Iniciando...";
    return true;
}

void OLEDDisplay::showMenu(int menuSelection) {
    display_.clearDisplay();
    display_.setCursor(0, 0);
    display_.println("Selecciona:");
    display_.setCursor(0, 16);
    display_.print(menuSelection == 0 ? "> " : "  ");
    display_.println("1. Reproducir SD");
    display_.setCursor(0, 24);
    display_.print(menuSelection == 1 ? "> " : "  ");
    display_.println("2. Subir cancion");
    display_.display();
    lastDisplayedText_ = ""; // Clear last text to avoid conflicts
}

void OLEDDisplay::showUploadNotification() {
    display_.clearDisplay();
    display_.setCursor(0, 0);
    display_.println("¡Cancion subida!");
    display_.display();
    lastDisplayedText_ = "¡Cancion subida!";
}

void OLEDDisplay::updateDisplay(const String& track, float volume, bool paused) {
    if (lastDisplayedTrack_ != track || lastDisplayedVolume_ != volume || lastDisplayedPaused_ != paused) {
        display_.clearDisplay();
        display_.setCursor(0, 0);
        display_.println(paused ? "Pausado" : "Reproduciendo:");
        display_.println(track.substring(1));
        display_.print("Vol: ");
        display_.println((int)(volume * 100));
        display_.display();
        lastDisplayedTrack_ = track;
        lastDisplayedVolume_ = volume;
        lastDisplayedPaused_ = paused;
        lastDisplayedText_ = "";
    }
}

void OLEDDisplay::showText(const String& text) {
    if (lastDisplayedText_ != text) { // Only update if text has changed
        display_.clearDisplay();
        display_.setCursor(0, 0);
        display_.println(text);
        display_.display();
        lastDisplayedText_ = text;
    }
}

void OLEDDisplay::clear() {
    display_.clearDisplay();
    display_.display();
    lastDisplayedText_ = "";
}