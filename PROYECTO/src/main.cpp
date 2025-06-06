#include <Arduino.h>
#include "MusicPlayer.h"

MusicPlayer musicPlayer;

void setup() {
    if (!musicPlayer.begin()) {
        Serial.println("Error al inicializar el reproductor");
        for (;;);
    }
}

void loop() {
    musicPlayer.loop();
}