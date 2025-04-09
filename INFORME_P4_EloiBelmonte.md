# Práctica 3 PD ELOI BELMONTE ALCALÁ (COMPAÑERO : ALVARO RAMO)

Esta práctica está centrada en los sistemas operativos en tiempo real, poniendo especial atención en cómo se gestionan y ejecutan múltiples tareas repartiéndose el tiempo de procesamiento entre ellas.

## Ej 1
```c++
#include <Arduino.h>
// Definim el pin del LED
const int ledPin = 2;

// Definim les tasques
void toggleLED(void * parameter) {
for(;;) {
digitalWrite(ledPin, HIGH);
vTaskDelay(500 / portTICK_PERIOD_MS);
digitalWrite(ledPin, LOW);
vTaskDelay(500 / portTICK_PERIOD_MS);
}
}

void anotherTask(void * parameter) {
for(;;) {
Serial.println("Això és una altra tasca");
vTaskDelay(1000 / portTICK_PERIOD_MS);
}
}

void setup() {
Serial.begin(115200);
pinMode(ledPin, OUTPUT);

// Crear la tasca del LED en el Core 0
xTaskCreatePinnedToCore(
toggleLED, "Toggle LED", 1000, NULL, 1, NULL, 0
);

// Crear la segona tasca en el Core 1
xTaskCreatePinnedToCore(
anotherTask, "Another Task", 1000, NULL, 1, NULL, 1
);
}

void loop() {
Serial.println("Això és la tasca principal (loop)");
delay(1000);
}
```
### Funcionamiento y output :
Funcionamiento y salida:
Este código demuestra cómo usar FreeRTOS en el ESP32 para ejecutar múltiples tareas de forma paralela utilizando distintos núcleos del procesador.

Tarea principal (loop): se ejecuta cada segundo y muestra el mensaje "Això és la tasca principal (loop)" por el puerto serie.

Primera tarea (toggleLED): hace parpadear un LED conectado al pin 2, alternando entre encendido y apagado cada 500 ms. Esta tarea se asigna al Core 0.

Segunda tarea (anotherTask): imprime "Això és una altra tasca" cada segundo en el monitor serie y se ejecuta en el Core 1.

Salida esperada por el monitor serie:

   Això és la tasca principal (loop)
   Això és una altra tasca

### Fotos y vídeos del ejercicio : 

https://drive.google.com/file/d/1zhchI7dclPk9HGiP9yRXbzS2POQ5s_nb/view?usp=drive_link

https://drive.google.com/file/d/1eegE6llPcSN5SoNt5cypAsA9-cbonc6B/view?usp=drive_link

## Ej 2 - Semáforo
```c++
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

const int ledPin = 2;

// Semàfors per sincronitzar les tasques
SemaphoreHandle_t semEncendre, semEsperar, semApagar;

// Tasca per encendre el LED
void encendreLED(void * parameter) {
while (1) {
xSemaphoreTake(semEncendre, portMAX_DELAY); // Espera el semàfor
digitalWrite(ledPin, HIGH);
Serial.println("LED ENCÈS");
vTaskDelay(10 / portTICK_PERIOD_MS); // Breu retard per garantir execució seqüencial
xSemaphoreGive(semEsperar); // Permet que l'espera comenci
}
}

// Tasca per esperar 1 segon
void esperarLED(void * parameter) {
while (1) {
xSemaphoreTake(semEsperar, portMAX_DELAY); // Espera el semàfor
Serial.println("Esperant...");
vTaskDelay(1000 / portTICK_PERIOD_MS); // Espera 1 segon
xSemaphoreGive(semApagar); // Permet que la següent tasca apagui el LED
}
}

// Tasca per apagar el LED
void apagarLED(void * parameter) {
while (1) {
xSemaphoreTake(semApagar, portMAX_DELAY); // Espera el semàfor
digitalWrite(ledPin, LOW);
Serial.println("LED APAGAT");
vTaskDelay(1000 / portTICK_PERIOD_MS); // Espera 1 segon abans de tornar a començar
xSemaphoreGive(semEncendre); // Permet que el procés es repeteixi
}
}

void setup() {
Serial.begin(115200);
pinMode(ledPin, OUTPUT);

// Crear semàfors
semEncendre = xSemaphoreCreateBinary();
semEsperar = xSemaphoreCreateBinary();
semApagar = xSemaphoreCreateBinary();

// Donem la primera execució al semàfor d'encendre
xSemaphoreGive(semEncendre);

// Crear tasques
xTaskCreate(encendreLED, "Encendre LED", 1000, NULL, 1, NULL);
xTaskCreate(esperarLED, "Esperar LED", 1000, NULL, 1, NULL);
xTaskCreate(apagarLED, "Apagar LED", 1000, NULL, 1, NULL);
}

void loop() {
// No fem res en el loop, FreeRTOS gestiona les tasques
}
```
### Funcionamiento :

Este código sincroniza tres tareas que controlan el encendido y apagado del LED, usando semáforos binarios para asegurar un orden de ejecución correcto:

encendreLED(): espera la señal del semáforo semEncendre, enciende el LED, muestra un mensaje por el puerto serie y luego activa semEsperar.

esperarLED(): se activa con semEsperar, espera 1 segundo y da paso a la tarea encargada de apagar el LED mediante semApagar.

apagarLED(): espera el semáforo correspondiente, apaga el LED, espera otro segundo y reinicia el ciclo activando de nuevo semEncendre.

El loop() queda vacío, ya que toda la lógica se gestiona a través de las tareas creadas por FreeRTOS.


## Ej 3 Subir Nota - Reloj rtos

```c++
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

// Definición de pines
#define LED_SEGUNDOS 2
#define LED_MODO 4
#define BTN_MODO 16
#define BTN_INCREMENTO 17

// Variables del reloj
volatile int horas = 0, minutos = 0, segundos = 0, modo = 0;

// Recursos de FreeRTOS
QueueHandle_t botonQueue;
SemaphoreHandle_t relojMutex;

// Estructura para eventos de botones
typedef struct {
  uint8_t boton;
  uint32_t tiempo;
} EventoBoton;

// ISR para los botones
void IRAM_ATTR ISR_Boton(void *arg) {
  EventoBoton evento = {(uint8_t)(uint32_t)arg, xTaskGetTickCountFromISR()};
  xQueueSendFromISR(botonQueue, &evento, NULL);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_SEGUNDOS, OUTPUT);
  pinMode(LED_MODO, OUTPUT);
  pinMode(BTN_MODO, INPUT_PULLUP);
  pinMode(BTN_INCREMENTO, INPUT_PULLUP);

  botonQueue = xQueueCreate(10, sizeof(EventoBoton));
  relojMutex = xSemaphoreCreateMutex();

  attachInterruptArg(BTN_MODO, ISR_Boton, (void*)BTN_MODO, FALLING);
  attachInterruptArg(BTN_INCREMENTO, ISR_Boton, (void*)BTN_INCREMENTO, FALLING);

  xTaskCreate(TareaReloj, "RelojTask", 2048, NULL, 1, NULL);
  xTaskCreate(TareaLecturaBotones, "BotonesTask", 2048, NULL, 2, NULL);
  xTaskCreate(TareaActualizacionDisplay, "DisplayTask", 2048, NULL, 1, NULL);
  xTaskCreate(TareaControlLEDs, "LEDsTask", 1024, NULL, 1, NULL);
}

void loop() { vTaskDelay(portMAX_DELAY); }

void TareaReloj(void *pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  for (;;) {
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(1000));
    if (xSemaphoreTake(relojMutex, portMAX_DELAY)) {
      if (modo == 0) {
        if (++segundos >= 60) { segundos = 0; if (++minutos >= 60) { minutos = 0; if (++horas >= 24) horas = 0; } }
      }
      xSemaphoreGive(relojMutex);
    }
  }
}

void TareaLecturaBotones(void *pvParameters) {
  EventoBoton evento;
  uint32_t ultimoTiempoBoton = 0, debounceTime = pdMS_TO_TICKS(300);
  for (;;) {
    if (xQueueReceive(botonQueue, &evento, portMAX_DELAY)) {
      if ((evento.tiempo - ultimoTiempoBoton) >= debounceTime) {
        if (xSemaphoreTake(relojMutex, portMAX_DELAY)) {
          if (evento.boton == BTN_MODO) modo = (modo + 1) % 3;
          else if (evento.boton == BTN_INCREMENTO) {
            if (modo == 1) horas = (horas + 1) % 24;
            else if (modo == 2) { minutos = (minutos + 1) % 60; segundos = 0; }
          }
          xSemaphoreGive(relojMutex);
        }
        ultimoTiempoBoton = evento.tiempo;
      }
    }
  }
}

void TareaActualizacionDisplay(void *pvParameters) {
  int hAnt = -1, mAnt = -1, sAnt = -1, mAntMode = -1;
  for (;;) {
    if (xSemaphoreTake(relojMutex, portMAX_DELAY)) {
      if (horas != hAnt || minutos != mAnt || segundos != sAnt || modo != mAntMode) {
        Serial.printf("%02d:%02d:%02d [%s]\n", horas, minutos, segundos, modo == 0 ? "Normal" : modo == 1 ? "Ajuste Horas" : "Ajuste Minutos");
        hAnt = horas; mAnt = minutos; sAnt = segundos; mAntMode = modo;
      }
      xSemaphoreGive(relojMutex);
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void TareaControlLEDs(void *pvParameters) {
  bool estadoLedSegundos = false;
  for (;;) {
    if (xSemaphoreTake(relojMutex, portMAX_DELAY)) {
      if (segundos != estadoLedSegundos) {
        estadoLedSegundos = !estadoLedSegundos;
        digitalWrite(LED_SEGUNDOS, estadoLedSegundos);
      }
      digitalWrite(LED_MODO, modo > 0);
      xSemaphoreGive(relojMutex);
    }
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}
```
### Funcionamiento :
Ejercicio 3 – Subir Nota: Reloj con RTOS
Funcionamiento:
Este programa implementa un reloj digital en tiempo real con posibilidad de ajuste mediante botones. Usa FreeRTOS para distribuir responsabilidades entre varias tareas:

TareaReloj: se encarga de incrementar la hora cada segundo, controlando correctamente el paso de segundos, minutos y horas. Solo actúa en modo normal.

TareaLecturaBotones: gestiona la lectura de los botones con antirrebote. Cambia el modo (normal, ajuste de horas, ajuste de minutos) o incrementa los valores de tiempo dependiendo del botón y el modo activo.

TareaActualizacionDisplay: actualiza la salida por puerto serie solo cuando hay cambios en el tiempo o en el modo, mostrando siempre la hora actual y el estado.

TareaControlLEDs: hace parpadear el LED de segundos y mantiene encendido el LED de modo si el usuario se encuentra en algún modo de ajuste.

Cada tarea está bien separada para cumplir con su función específica, lo cual demuestra una correcta aplicación de los conceptos multitarea con sincronización y exclusión mutua en un sistema embebido.