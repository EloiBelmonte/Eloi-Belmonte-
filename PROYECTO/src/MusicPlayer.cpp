#include "MusicPlayer.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SD_CS 10
#define SPI_MOSI 11
#define SPI_MISO 13
#define SPI_SCK 12
#define I2S_BCLK 38
#define I2S_LRCLK 37
#define I2S_DIN 39
#define BUTTON_VOLUME_UP 5
#define BUTTON_VOLUME_DOWN 6
#define BUTTON_PAUSE 7
#define BUTTON_RESTART 8
#define BUTTON_SKIP 9
#define IR_RECEIVE_PIN 4
#define IR_CODE_VOLUME_UP 0xFFA857
#define IR_CODE_VOLUME_DOWN 0xFFE01F
#define IR_CODE_PAUSE 0xFFC23D
#define IR_CODE_RESTART 0xFF22DD
#define IR_CODE_SKIP 0xFF02FD
#define IR_CODE_REPEAT 0xFFFFFFFF
#define IR_CODE_BUTTON_1 0xFF30CF
#define IR_CODE_BUTTON_2 0xFF18E7
const char* ssid = "iPhone";
const char* password = "PereLopez777";

MusicPlayer::MusicPlayer()
    : display_(new OLEDDisplay(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_RESET)),
      sdCard_(new SDCard(SD_CS, SPI_MOSI, SPI_MISO, SPI_SCK)),
      audioPlayer_(new AudioPlayer(I2S_BCLK, I2S_LRCLK, I2S_DIN)),
      irReceiver_(new IRReceiver(IR_RECEIVE_PIN)),
      buttonManager_(new ButtonManager(BUTTON_VOLUME_UP, BUTTON_VOLUME_DOWN, BUTTON_PAUSE, BUTTON_RESTART, BUTTON_SKIP)),
      webServer_(new WebServer(ssid, password, sdCard_, display_)),
      inMenu_(true),
      menuSelection_(0),
      currentTrackIndex_(0),
      volume_(0.1),
      paused_(false),
      playlistFinished_(false),
      lastValidPress_(0),
      lastIRPressTime_(0),
      lastIRCode_(0) {}

bool MusicPlayer::begin() {
    Serial.begin(115200);
    delay(1000);
    Wire.begin(47, 48);
    if (!display_->begin()) {
        Serial.println("Error al inicializar la pantalla SSD1306");
        return false;
    }
    if (!sdCard_->begin()) {
        Serial.println("Error al inicializar la SD, continuando sin SD...");
    }
    if (!SPIFFS.begin(true)) {
        Serial.println("Error al inicializar SPIFFS");
    }
    audioPlayer_->begin();
    irReceiver_->begin();
    buttonManager_->begin();
    display_->showMenu(menuSelection_);
    return true;
}

void MusicPlayer::loop() {
    unsigned long currentTime = millis();

    if (buttonManager_->isRestartPressed() && (currentTime - lastValidPress_) > minPressInterval_) {
        lastValidPress_ = currentTime;
        returnToMenu();
        Serial.println("Volviendo al menú principal...");
    }

    if (inMenu_) {
        handleMenu();
    } else {
        if (menuSelection_ == 0) {
            handlePlayback();
        } else {
            handleWebMode();
        }
    }
}

void MusicPlayer::returnToMenu() {
    audioPlayer_->stop();
    webServer_->end();
    inMenu_ = true;
    menuSelection_ = 0;
    currentTrackIndex_ = 0;
    paused_ = false;
    playlistFinished_ = false;
    display_->showMenu(menuSelection_);
}

void MusicPlayer::handleMenu() {
    unsigned long currentTime = millis();
    if (buttonManager_->isVolumeUpPressed() && (currentTime - lastValidPress_) > minPressInterval_) {
        lastValidPress_ = currentTime;
        menuSelection_--;
        if (menuSelection_ < 0) menuSelection_ = 0;
        display_->showMenu(menuSelection_);
    }
    if (buttonManager_->isVolumeDownPressed() && (currentTime - lastValidPress_) > minPressInterval_) {
        lastValidPress_ = currentTime;
        menuSelection_++;
        if (menuSelection_ > 1) menuSelection_ = 1;
        display_->showMenu(menuSelection_);
    }
    if (buttonManager_->isPausePressed() && (currentTime - lastValidPress_) > minPressInterval_) {
        lastValidPress_ = currentTime;
        inMenu_ = false;
        if (menuSelection_ == 0) {
            sdCard_->loadPlaylist();
            if (!sdCard_->getPlaylist().empty()) {
                audioPlayer_->playTrack(sdCard_->getPlaylist()[currentTrackIndex_]);
                display_->updateDisplay(sdCard_->getPlaylist()[currentTrackIndex_], volume_, paused_);
            } else {
                display_->showText("No MP3 encontrados");
                delay(2000);
                returnToMenu();
            }
        } else {
            if (!webServer_->begin()) {
                delay(2000); // Wait to show "Error Wi-Fi" from WebServer::begin()
                returnToMenu();
            } else {
                display_->showText("Modo Web\nConectate a:\n" + String(ssid) + "\nIP: " + webServer_->getLocalIP());
            }
        }
    }
    unsigned long irCode = irReceiver_->getIRCode();
    if (irCode) {
        if (irCode == IR_CODE_BUTTON_1) {
            menuSelection_ = 0;
            inMenu_ = false;
            sdCard_->loadPlaylist();
            if (!sdCard_->getPlaylist().empty()) {
                audioPlayer_->playTrack(sdCard_->getPlaylist()[currentTrackIndex_]);
                display_->updateDisplay(sdCard_->getPlaylist()[currentTrackIndex_], volume_, paused_);
            } else {
                display_->showText("No MP3 encontrados");
                delay(2000);
                returnToMenu();
            }
        } else if (irCode == IR_CODE_BUTTON_2) {
            menuSelection_ = 1;
            inMenu_ = false;
            if (!webServer_->begin()) {
                delay(2000); // Wait to show "Error Wi-Fi" from WebServer::begin()
                returnToMenu();
            } else {
                display_->showText("Modo Web\nConéctate a:\n" + String(ssid) + "\nIP: " + webServer_->getLocalIP());
            }
        }
    }
}

void MusicPlayer::handlePlayback() {
    unsigned long currentTime = millis();
    if (buttonManager_->isVolumeUpPressed() && (currentTime - lastValidPress_) > minPressInterval_) {
        lastValidPress_ = currentTime;
        volume_ += 0.05;
        if (volume_ > 1.0) volume_ = 1.0;
        audioPlayer_->setVolume(volume_);
        Serial.print("Volumen subido: ");
        Serial.println(volume_);
        display_->updateDisplay(sdCard_->getPlaylist()[currentTrackIndex_], volume_, paused_);
    }
    if (buttonManager_->isVolumeDownPressed() && (currentTime - lastValidPress_) > minPressInterval_) {
        lastValidPress_ = currentTime;
        volume_ -= 0.05;
        if (volume_ < 0.0) volume_ = 0.0;
        audioPlayer_->setVolume(volume_);
        Serial.print("Volumen bajado: ");
        Serial.println(volume_);
        display_->updateDisplay(sdCard_->getPlaylist()[currentTrackIndex_], volume_, paused_);
    }
    if (buttonManager_->isPausePressed() && (currentTime - lastValidPress_) > minPressInterval_) {
        lastValidPress_ = currentTime;
        paused_ = !paused_;
        if (paused_) {
            audioPlayer_->pause();
            Serial.println("Pausado");
        } else {
            audioPlayer_->resume(sdCard_->getPlaylist()[currentTrackIndex_]);
            Serial.println("Reanudado");
        }
        display_->updateDisplay(sdCard_->getPlaylist()[currentTrackIndex_], volume_, paused_);
    }
    if (buttonManager_->isSkipPressed() && (currentTime - lastValidPress_) > minPressInterval_) {
        lastValidPress_ = currentTime;
        currentTrackIndex_++;
        if (currentTrackIndex_ >= sdCard_->getPlaylist().size()) {
            display_->showText("Fin de la playlist");
            playlistFinished_ = true;
            audioPlayer_->stop();
            return;
        }
        audioPlayer_->playTrack(sdCard_->getPlaylist()[currentTrackIndex_]);
        display_->updateDisplay(sdCard_->getPlaylist()[currentTrackIndex_], volume_, paused_);
        Serial.println("Saltando a la siguiente cancion...");
    }
    unsigned long irCode = irReceiver_->getIRCode();
    if (irCode) {
        if (irCode == IR_CODE_REPEAT && (currentTime - lastIRPressTime_) < 500) {
            if (lastIRCode_ == IR_CODE_VOLUME_UP) {
                volume_ += 0.05;
                if (volume_ > 1.0) volume_ = 1.0;
                audioPlayer_->setVolume(volume_);
                Serial.print("Volumen subido (IR repetido): ");
                Serial.println(volume_);
                display_->updateDisplay(sdCard_->getPlaylist()[currentTrackIndex_], volume_, paused_);
                lastIRPressTime_ = currentTime;
            } else if (lastIRCode_ == IR_CODE_VOLUME_DOWN) {
                volume_ -= 0.05;
                if (volume_ < 0.0) volume_ = 0.0;
                audioPlayer_->setVolume(volume_);
                Serial.print("Volumen bajado (IR repetido): ");
                Serial.println(volume_);
                display_->updateDisplay(sdCard_->getPlaylist()[currentTrackIndex_], volume_, paused_);
                lastIRPressTime_ = currentTime;
            }
        } else if (currentTime - lastValidPress_ > minPressInterval_) {
            lastValidPress_ = currentTime;
            lastIRPressTime_ = currentTime;
            lastIRCode_ = irCode;
            switch (irCode) {
                case IR_CODE_VOLUME_UP:
                    volume_ += 0.05;
                    if (volume_ > 1.0) volume_ = 1.0;
                    audioPlayer_->setVolume(volume_);
                    Serial.print("Volumen subido (IR): ");
                    Serial.println(volume_);
                    display_->updateDisplay(sdCard_->getPlaylist()[currentTrackIndex_], volume_, paused_);
                    break;
                case IR_CODE_VOLUME_DOWN:
                    volume_ -= 0.05;
                    if (volume_ < 0.0) volume_ = 0.0;
                    audioPlayer_->setVolume(volume_);
                    Serial.print("Volumen bajado (IR): ");
                    Serial.println(volume_);
                    display_->updateDisplay(sdCard_->getPlaylist()[currentTrackIndex_], volume_, paused_);
                    break;
                case IR_CODE_PAUSE:
                    paused_ = !paused_;
                    if (paused_) {
                        audioPlayer_->pause();
                        Serial.println("Pausado (IR)");
                    } else {
                        audioPlayer_->resume(sdCard_->getPlaylist()[currentTrackIndex_]);
                        Serial.println("Reanudado (IR)");
                    }
                    display_->updateDisplay(sdCard_->getPlaylist()[currentTrackIndex_], volume_, paused_);
                    break;
                case IR_CODE_RESTART:
                    Serial.println("Volviendo al menú principal (IR)...");
                    returnToMenu();
                    break;
                case IR_CODE_SKIP:
                    currentTrackIndex_++;
                    if (currentTrackIndex_ >= sdCard_->getPlaylist().size()) {
                        display_->showText("Fin de la playlist");
                        playlistFinished_ = true;
                        audioPlayer_->stop();
                        return;
                    }
                    audioPlayer_->playTrack(sdCard_->getPlaylist()[currentTrackIndex_]);
                    display_->updateDisplay(sdCard_->getPlaylist()[currentTrackIndex_], volume_, paused_);
                    Serial.println("Saltando a la siguiente cancion (IR)...");
                    break;
            }
        }
    }
    if (!paused_ && audioPlayer_->isRunning()) {
        if (!audioPlayer_->loop()) {
            audioPlayer_->stop();
            Serial.println("Reproducción finalizada: " + sdCard_->getPlaylist()[currentTrackIndex_]);
            currentTrackIndex_++;
            if (currentTrackIndex_ >= sdCard_->getPlaylist().size()) {
                display_->showText("Fin de la playlist");
                playlistFinished_ = true;
                return;
            }
            audioPlayer_->playTrack(sdCard_->getPlaylist()[currentTrackIndex_]);
            display_->updateDisplay(sdCard_->getPlaylist()[currentTrackIndex_], volume_, paused_);
        }
    }
}

void MusicPlayer::handleWebMode() {
    unsigned long currentTime = millis();
    if (buttonManager_->isPausePressed() && (currentTime - lastValidPress_) > minPressInterval_) {
        lastValidPress_ = currentTime;
        webServer_->end();
        inMenu_ = true;
        display_->showMenu(menuSelection_);
    }
    // Check Wi-Fi status periodically
    if (webServer_->isActive() && WiFi.status() != WL_CONNECTED) {
        Serial.println("Conexión Wi-Fi perdida, volviendo al menú...");
        webServer_->end();
        display_->showText("Wi-Fi perdido");
        delay(2000);
        returnToMenu();
    }
}