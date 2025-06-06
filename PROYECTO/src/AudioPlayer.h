#ifndef AUDIO_PLAYER_H
#define AUDIO_PLAYER_H

#include <Arduino.h>
#include "AudioFileSourceSD.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"

class AudioPlayer {
public:
    AudioPlayer(int bclkPin, int lrclkPin, int dinPin);
    bool begin();
    void playTrack(const String& track);
    void stop();
    void setVolume(float volume);
    bool isRunning();
    bool loop();
    void pause();
    void resume(const String& track);

private:
    int bclkPin_;
    int lrclkPin_;
    int dinPin_;
    AudioGeneratorMP3* mp3_;
    AudioFileSourceSD* file_;
    AudioOutputI2S* out_;
    float volume_;
    bool paused_;
};

#endif