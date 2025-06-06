#ifndef SD_CARD_H
#define SD_CARD_H

#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include <vector>

class SDCard {
public:
    SDCard(int csPin, int mosiPin, int misoPin, int sckPin);
    bool begin();
    std::vector<String>& getPlaylist();
    void loadPlaylist();

private:
    int csPin_;
    int mosiPin_;
    int misoPin_;
    int sckPin_;
    std::vector<String> playlist_;
};

#endif