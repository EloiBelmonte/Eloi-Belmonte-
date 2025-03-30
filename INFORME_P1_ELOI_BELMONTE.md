# Práctica 1 PD - Eloi Belmonte (Compañero: Álvaro Ramo Irurre)

# Introducción 

# Esta práctica tiene como objetivo programar un ESP32 para controlar el parpadeo de un LED de manera periódica.

# 1. Código Básico

# A continuación, se presenta un fragmento de código que implementa la funcionalidad básica para hacer que un LED parpadee en el ESP32.

#define LED_BUILTIN 2
#define DELAY 500

void setup() {
 pinMode(LED_BUILTIN, OUTPUT);
}
void loop() {
 digitalWrite(LED_BUILTIN, HIGH);
 delay(DELAY);
 digitalWrite(LED_BUILTIN, LOW);
 delay(DELAY);
}

# 2. Modificación del programa (ON, OFF)

#include <Adafruit_NeoPixel.h>

#define LED_PIN 48    // Cambiar a 8, 13 o el pin correspondiente para el LED RGB
#define NUM_LEDS 1

Adafruit_NeoPixel led(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
 led.begin();
 led.setBrightness(50);  // Ajustar el brillo del LED
}

void loop() {
 led.setPixelColor(0, led.Color(255, 0, 0));  // LED en rojo
 led.show();
 delay(500);

 led.setPixelColor(0, led.Color(0, 0, 0));  // LED en apagado
 led.show();
 delay(500);
}

# Este ejemplo modifica el código para lograr un parpadeo del LED, permitiendo visualizar su comportamiento de una manera diferente.

# 3. Modificación para acceder a los registros de entrada y salida

#include <Arduino.h>
#define LED_PIN 2
#define DELAY 1000

void setup() {
    pinMode(LED_PIN, OUTPUT);
    Serial.begin(115200);
}

void loop() {
    volatile uint32_t *gpio_out = (uint32_t *)GPIO_OUT_REG;  // Acceder al registro de salida

    Serial.println("ON");
    *gpio_out |= (1 << LED_PIN);  // Encender LED
    delay(DELAY);

    Serial.println("OFF");
    *gpio_out &= ~(1 << LED_PIN); // Apagar LED
    delay(DELAY);
}

# Este fragmento de código se ha modificado para controlar directamente los registros de entrada y salida, optimizando el manejo del LED.

# 4. Medición de la frecuencia máxima

# En esta parte de la práctica, se cambiará el pin de salida y se utilizará un osciloscopio para medir la frecuencia máxima de conmutación del LED en distintas condiciones.

# 4.1. Con envío de mensajes por puerto serie y utilizando funciones de Arduino

#include <Arduino.h>

int led = 14;

void setup() {                
   pinMode(led, OUTPUT);   
   Serial.begin(115200);
}

void loop() {
   Serial.println("ON");
   digitalWrite(led, HIGH);
   Serial.println("OFF");      
   digitalWrite(led, LOW);
}

# En este caso, usando el pin 14 como salida, la frecuencia registrada en el osciloscopio es de 30 kHz.

# 4.2. Con envío por puerto serie y accediendo directamente a los registros

#include <Arduino.h>

int led = 14;
uint32_t *gpio_out = (uint32_t *)GPIO_OUT_REG;

void setup() {                
   pinMode(led, OUTPUT);   
   Serial.begin(115200);
}

void loop() {
   Serial.println("ON");
   *gpio_out |= (1 << led);
   Serial.println("OFF");      
   *gpio_out ^= (1 << led);
}

# Usando el mismo pin 14, se obtiene nuevamente una frecuencia de 30 kHz.

# 4.3. Sin envío por puerto serie y utilizando funciones de Arduino

#include <Arduino.h>
int led = 14;

void setup() {                
   pinMode(led, OUTPUT);   
}

void loop() {
   digitalWrite(led, HIGH);
   digitalWrite(led, LOW);
}

# En esta situación, la frecuencia medida es de 1.7 MHz.

# 4.4. Sin envío por puerto serie y accediendo directamente a los registros

#include <Arduino.h>

int led = 14;
uint32_t *gpio_out = (uint32_t *)GPIO_OUT_REG;

void setup() {                
   pinMode(led, OUTPUT);   
}

void loop() {
   *gpio_out |= (1 << led);
   *gpio_out ^= (1 << led);
}

# En este último escenario, el osciloscopio muestra una frecuencia de 4.7 MHz.

# 5. Diagramas

# Se presentan a continuación los diagramas de flujo y de tiempo que explican el funcionamiento del código básico.

# 5.1. Diagrama de flujo

# graph TD
#  A[Inicio] -->|Inicialización| B(Setup)
#  B -->|Configurar salida| C{LED Encendido}
#  C -->|Encender LED| D[Delay 500ms]
#  D -->|Apagar LED| E{LED Apagado}
#  E -->|Delay 500ms| D
#  E -->|Repetir| F[Fin]

# El diagrama de flujo ilustra el proceso de ejecución del programa, mostrando el encendido y apagado del LED con un retraso definido.

# 5.2. Diagrama de tiempo

# sequenceDiagram
#    participant Arduino
#    participant LED
#    participant Delay

#    Arduino->>Arduino: #define LED_BUILTIN 2
#    Arduino->>Arduino: #define DELAY 500
#    Arduino->>LED: pinMode(LED_BUILTIN, PUERTO_SALIDA)

#    loop
#        Arduino->>LED: digitalWrite(LED_BUILTIN, ENCENDIDO)
#        Arduino->>Delay: delay(DELAY)
#        Arduino->>LED: digitalWrite(LED_BUILTIN, APAGADO)
#        Arduino->>Delay: delay(DELAY)
#    end

# Este diagrama muestra la secuencia de eventos en la ejecución del programa.

# 6. Cálculo del tiempo libre del procesador

# El tiempo libre del procesador se determina restando el tiempo ocupado en ejecutar el bucle del tiempo total del ciclo. Conociendo ambos valores, es posible calcular el tiempo de inactividad del procesador.

