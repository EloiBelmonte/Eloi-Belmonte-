#include "AudioPlayer.h"

AudioPlayer::AudioPlayer(int bclkPin, int lrclkPin, int dinPin)
    : bclkPin_(bclkPin), lrclkPin_(lrclkPin), dinPin_(dinPin), mp3_(nullptr), file_(nullptr), out_(nullptr), volume_(0.1), paused_(false) {}

bool AudioPlayer::begin() {
    out_ = new AudioOutputI2S();
    out_->SetPinout(bclkPin_, lrclkPin_, dinPin_);
    out_->SetRate(44100);
    out_->SetBitsPerSample(16);
    out_->SetChannels(2);
    out_->SetGain(volume_);
    return true;
}

void AudioPlayer::playTrack(const String& track) {
    stop();
    file_ = new AudioFileSourceSD(track.c_str());
    if (!file_->isOpen()) {
        Serial.println("No se pudo abrir el archivo: " + track);
        return;
    }
    mp3_ = new AudioGeneratorMP3();
    mp3_->begin(file_, out_);
    out_->SetGain(volume_);
    paused_ = false;
}

void AudioPlayer::stop() {
    if (mp3_ && mp3_->isRunning()) {
        mp3_->stop();
    }
    if (file_) {
        delete file_;
        file_ = nullptr;
    }
    if (mp3_) {
        delete mp3_;
        mp3_ = nullptr;
    }
}

void AudioPlayer::setVolume(float volume) {
    volume_ = volume;
    if (out_) {
        out_->SetGain(volume_);
    }
}

bool AudioPlayer::isRunning() {
    return mp3_ && mp3_->isRunning();
}

bool AudioPlayer::loop() {
    if (!paused_ && mp3_ && mp3_->isRunning()) {
        return mp3_->loop();
    }
    return false;
}

void AudioPlayer::pause() {
    if (mp3_ && mp3_->isRunning()) {
        mp3_->stop();
        paused_ = true;
    }
}

void AudioPlayer::resume(const String& track) {
    if (paused_) {
        playTrack(track);
    }
}