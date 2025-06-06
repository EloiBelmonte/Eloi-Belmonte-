#include "SDCard.h"

SDCard::SDCard(int csPin, int mosiPin, int misoPin, int sckPin)
    : csPin_(csPin), mosiPin_(mosiPin), misoPin_(misoPin), sckPin_(sckPin) {}

bool SDCard::begin() {
    SPI.begin(sckPin_, misoPin_, mosiPin_);
    if (!SD.begin(csPin_)) {
        Serial.println("Error al inicializar la SD");
        return false;
    }
    Serial.println("SD inicializada");
    return true;
}

std::vector<String>& SDCard::getPlaylist() {
    return playlist_;
}

void SDCard::loadPlaylist() {
    playlist_.clear();
    File root = SD.open("/");
    while (true) {
        File entry = root.openNextFile();
        if (!entry) {
            break;
        }
        String filename = entry.name();
        if (filename.endsWith(".mp3") || filename.endsWith(".MP3")) {
            playlist_.push_back("/" + filename);
            Serial.println("Encontrado: " + filename);
        }
        entry.close();
    }
    root.close();
}