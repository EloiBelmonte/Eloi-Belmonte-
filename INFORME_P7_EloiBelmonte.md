# Práctica 7 PD ELOI BELMONTE (COMPAÑERO : ALVARO RAMO)

En esta séptima práctica trabajamos con el bus de comunicación en serie I2S, utilizado para enviar datos de audio digital de forma eficiente entre dispositivos.

## Ej 1

```c++

#include "Arduino.h"
#include "driver/i2s.h"
#include <math.h>

// 🎛 Pines de I2S en ESP32-S3 DevKitC-1
#define I2S_BCLK 7   // Bit Clock
#define I2S_LRC 6    // Word Select (LR Clock)
#define I2S_DOUT 5   // Datos de audio

#define SAMPLE_RATE 44100  // Frecuencia de muestreo
#define TONE_FREQ 440     // Frecuencia del tono (Hz)
#define AMPLITUDE 30000    // Amplitud de la onda (0 - 32767)
#define BUFFER_SIZE 128    // Tamaño del buffer de audio

int16_t audio_buffer[BUFFER_SIZE];  // Buffer para la señal de audio

void setup() {
    Serial.begin(115200);
    Serial.println("🔊 Generando tono de audio...");

    // Configuración de I2S
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_I2S,
        .intr_alloc_flags = 0,
        .dma_buf_count = 8,
        .dma_buf_len = BUFFER_SIZE,
        .use_apll = false
    };

    i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_BCLK,
        .ws_io_num = I2S_LRC,
        .data_out_num = I2S_DOUT,
        .data_in_num = I2S_PIN_NO_CHANGE
    };

    // Inicializa I2S
    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pin_config);
    i2s_zero_dma_buffer(I2S_NUM_0);
}

void loop() {
    static float phase = 0;
    float phase_increment = 2.0 * M_PI * TONE_FREQ / SAMPLE_RATE;

    // Genera la onda sinusoidal en el buffer
    for (int i = 0; i < BUFFER_SIZE; i++) {
        audio_buffer[i] = (int16_t)(AMPLITUDE * sin(phase));
        phase += phase_increment;
        if (phase >= 2.0 * M_PI) {
            phase -= 2.0 * M_PI;
        }
    }

    // Envía el buffer de audio a I2S
    size_t bytes_written;
    i2s_write(I2S_NUM_0, audio_buffer, sizeof(audio_buffer), &bytes_written, portMAX_DELAY);
}

```
### Funcionamiento y output :

Este programa genera y transmite una onda senoidal a través de la interfaz I2S del ESP32. La onda tiene una frecuencia de 440 Hz (tono LA4) y se produce en tiempo real utilizando funciones matemáticas. Se configura el bus I2S con una frecuencia de muestreo de 44100 Hz y resolución de 16 bits. El tono se transmite mediante un buffer circular de audio al dispositivo de salida conectado al ESP32.

Salida esperada:

Se escucha un tono constante (senoidal) en los altavoces o amplificador conectado al DAC I2S del ESP32.


## Ej 2 - RFID

```c++
#include "Audio.h"
#include "SD.h"
#include "FS.h"

// Digital I/O used
#define SD_CS          5
#define SPI_MOSI      23
#define SPI_MISO      19
#define SPI_SCK       18
#define I2S_DOUT      25
#define I2S_BCLK      27
#define I2S_LRC       26

Audio audio;

void setup(){
    pinMode(SD_CS, OUTPUT);
    digitalWrite(SD_CS, HIGH);
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
    Serial.begin(115200);
    SD.begin(SD_CS);
    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(10); // 0...21
    audio.connecttoFS(SD, "/SD card/Saiko.wav");
}

void loop(){
    audio.loop();
}

// optional
void audio_info(const char *info){
    Serial.print("info        "); Serial.println(info);
}
void audio_id3data(const char *info){  //id3 metadata
    Serial.print("id3data     ");Serial.println(info);
}
void audio_eof_mp3(const char *info){  //end of file
    Serial.print("eof_mp3     ");Serial.println(info);
}
void audio_showstation(const char *info){
    Serial.print("station     ");Serial.println(info);
}
void audio_showstreaminfo(const char *info){
    Serial.print("streaminfo  ");Serial.println(info);
}
void audio_showstreamtitle(const char *info){
    Serial.print("streamtitle ");Serial.println(info);
}
void audio_bitrate(const char *info){
    Serial.print("bitrate     ");Serial.println(info);
}
void audio_commercial(const char *info){  //duration in sec
    Serial.print("commercial  ");Serial.println(info);
}
void audio_icyurl(const char *info){  //homepage
    Serial.print("icyurl      ");Serial.println(info);
}
void audio_lasthost(const char *info){  //stream URL played
    Serial.print("lasthost    ");Serial.println(info);
}
void audio_eof_speech(const char *info){
    Serial.print("eof_speech  ");Serial.println(info);
}
```
### Funcionamiento y output :
Este código permite reproducir un archivo de audio (por ejemplo, un archivo .wav) almacenado en una tarjeta SD. Primero se inicializa la comunicación SPI para acceder a la tarjeta y luego se establece la configuración de pines y volumen para el módulo de audio. La función audio.loop() se encarga de mantener la reproducción en curso.

Además, se incluyen varias funciones auxiliares que capturan y muestran diferentes tipos de eventos o información relacionados con la reproducción de audio. Estas funciones imprimen detalles por el monitor serie como metadatos, finalización de pista, título del flujo o tasa de bits, entre otros.

Salida esperada:

Se reproduce el archivo de audio desde la tarjeta SD a través del I2S.

El monitor serie mostrará información detallada sobre el archivo de audio durante la reproducción, dependiendo de lo que esté ocurriendo.

### Descripción de funciones informativas:
 - ### audio_info: 
 Muestra mensajes genéricos del estado de reproducción.

 - ### audio_id3data: 
 Extrae y muestra metadatos como el título o artista del archivo.

 - ### audio_eof_mp3: 
 Notifica cuando un archivo MP3 ha terminado.

 - ### audio_showstation: 
 Imprime el nombre de la emisora si se está reproduciendo streaming.

 - ### audio_showstreaminfo: 
 Proporciona detalles del flujo de audio que se está recibiendo.

 - ### audio_showstreamtitle: 
 Muestra el título actual del flujo, útil para transmisiones en vivo.

 - ### audio_bitrate: 
 Informa la tasa de bits del archivo o flujo.

 - ### audio_commercial:
Indica la duración de los anuncios en segundos.

 - ### audio_icyurl: 
 Muestra el enlace web asociado a la emisora o flujo.

 - ### audio_lasthost: 
 Muestra la última URL reproducida desde la red.

 - ### audio_eof_speech:
  Señala la finalización de un archivo de voz o discurso.
### Fotos del laboratorio :

Montaje : 

https://drive.google.com/file/d/1gI2faoIOdrOIkDJNDiMEWiy7BvDEZQQx/view?usp=drive_link