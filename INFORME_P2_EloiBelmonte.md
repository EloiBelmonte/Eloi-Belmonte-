# Práctica 2 PD

En este ejercicio, se investiga cómo usar las interrupciones en microcontroladores para manejar eventos de manera más eficiente, evitando depender del polling. Se implementan interrupciones tanto a través de GPIO como mediante temporizadores en un ESP32.

**- Práctica A:**
Aquí se configurará un botón para generar interrupciones y contar cuántas veces se ha pulsado, respondiendo en tiempo real.

**- Práctica B:**
En esta sección, se usará un temporizador interno para generar interrupciones de forma periódica y contar cuántas veces se producen.

# 1. Parte A
```c
#include <Arduino.h>

struct Button {
  const uint8_t PIN;
  uint32_t numberKeyPresses;
  bool pressed;
  uint32_t lastDebounceTime;
};

Button button1 = {18, 0, false, 0};
const uint32_t debounceDelay = 200; // Aumentamos el tiempo de debounce

void IRAM_ATTR isr() { 
  detachInterrupt(button1.PIN);  // Desactiva la interrupción temporalmente
  button1.pressed = true;
}

void setup() { 
  Serial.begin(115200); 

  pinMode(button1.PIN, INPUT_PULLUP);
  attachInterrupt(button1.PIN, isr, FALLING); // Interrupción en flanco de bajada
  
  pinMode(5, OUTPUT);  // LED1 en GPIO5
  pinMode(4, OUTPUT);  // LED2 en GPIO4
} 

void loop() { 
  if (button1.pressed) { 
    uint32_t currentMillis = millis();
    if (currentMillis - button1.lastDebounceTime > debounceDelay) { // Verifica debounce
      button1.numberKeyPresses++;
      Serial.printf("Button 1 has been pressed %u times\n", button1.numberKeyPresses);
      button1.lastDebounceTime = currentMillis;

      // Cambio en la frecuencia de parpadeo del LED 1
      for (int i = 0; i < 5; i++) {
        digitalWrite(5, !digitalRead(5));
        delay(5);
      }
    }

    button1.pressed = false;
    attachInterrupt(button1.PIN, isr, FALLING);  // Reactiva la interrupción
  } 

  // Parpadeo del LED 2
  digitalWrite(4, !digitalRead(4));  
  delay(5);  
}
```
### Funcionamiento y salidas:

En este código, se implementa una interrupción en el pin GPIO de un ESP32 para detectar pulsaciones de un botón y, dependiendo de la cantidad de pulsaciones, ajustar la velocidad de parpadeo de un LED.

Primero, se define una estructura para almacenar información sobre el botón, como el número de pulsaciones, si está presionado y el tiempo de la última activación, para así evitar que los rebotes del botón interfieran. En la rutina de interrupción (isr()), se desactiva momentáneamente la interrupción y se marca el botón como presionado.

Dentro de la función setup(), se configura el pin del botón como una entrada con resistencia pull-up y se asigna la interrupción para activarse cuando el botón se presione (flanco descendente). También se configuran dos pines para los LEDs.

En el bucle principal (loop()), se verifica si el botón ha sido presionado. Si es así, se asegura que haya pasado suficiente tiempo desde la última pulsación para evitar los rebotes. Si la pulsación es válida, se incrementa el contador de pulsaciones y se ajusta el parpadeo del LED 1. Mientras tanto, el LED 2 parpadea de manera continua.

Finalmente, después de procesar la pulsación, se vuelve a habilitar la interrupción para detectar futuras presiones.

Salida:
El botón 1 ha sido presionado 1 veces
El botón 1 ha sido presionado 2 veces
El botón 1 ha sido presionado 3 veces
El botón 1 ha sido presionado 4 veces

# 2. Parte B
```c
volatile int interruptCounter;
int totalInterruptCounter;
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
void IRAM_ATTR onTimer() {
portENTER_CRITICAL_ISR(&timerMux);
interruptCounter++;
portEXIT_CRITICAL_ISR(&timerMux);
}
void setup() {
Serial.begin(115200);
timer = timerBegin(0, 80, true);
timerAttachInterrupt(timer, &onTimer, true);
timerAlarmWrite(timer, 1000000, true);
timerAlarmEnable(timer);
}
void loop() {
if (interruptCounter > 0) {
portENTER_CRITICAL(&timerMux);
interruptCounter--;
portEXIT_CRITICAL(&timerMux);
totalInterruptCounter++;
Serial.print("An interrupt as occurred. Total number: ");
Serial.println(totalInterruptCounter);
}
}
```
### Funcionamiento y salidas:

En esta parte de la práctica, se utiliza un temporizador para generar interrupciones a intervalos regulares. El temporizador está configurado para disparar una interrupción cada 1 segundo (1,000,000 microsegundos). Cada vez que se genera una interrupción, se incrementa un contador. Luego, en el bucle principal (loop()), se imprime el número total de interrupciones que han ocurrido.

El temporizador se configura con un preescalador y se adjunta la rutina de interrupción, la cual se ejecutará cada vez que el temporizador llegue a su valor límite.

La salida por el puerto serie es: 

An interrupt as occurred. Total number: 1
An interrupt as occurred. Total number: 2
An interrupt as occurred. Total number: 3

