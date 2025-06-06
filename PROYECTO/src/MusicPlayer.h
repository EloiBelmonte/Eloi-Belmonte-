#ifndef MUSIC_PLAYER_H
#define MUSIC_PLAYER_H

#include <Arduino.h>
#include "OLEDDisplay.h"
#include "SDCard.h"
#include "AudioPlayer.h"
#include "IRReceiver.h"
#include "ButtonManager.h"
#include "WebServer.h"

class MusicPlayer {
public:
    MusicPlayer();
    bool begin();
    void loop();

private:
    OLEDDisplay* display_;
    SDCard* sdCard_;
    AudioPlayer* audioPlayer_;
    IRReceiver* irReceiver_;
    ButtonManager* buttonManager_;
    WebServer* webServer_;
    bool inMenu_;
    int menuSelection_;
    int currentTrackIndex_;
    float volume_;
    bool paused_;
    bool playlistFinished_;
    unsigned long lastValidPress_;
    unsigned long lastIRPressTime_;
    unsigned long lastIRCode_;
    const unsigned long minPressInterval_ = 300;
    const unsigned long volumeAdjustInterval_ = 200;
    void returnToMenu();
    void handleMenu();
    void handlePlayback();
    void handleWebMode();
};

#endif