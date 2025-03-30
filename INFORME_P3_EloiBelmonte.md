# Práctica 3 PD wifi y bluetooth, ELOI BELMONTE  (COMPAÑERO : ALVARO RAMO)

## Introducción de la práctica
El propósito de esta práctica es trabajar con las conexiones WiFi y Bluetooth utilizando el microprocesador ESP32, que ya se ha empleado en prácticas anteriores. En esta actividad, configuraremos un servidor web en la placa ESP32 y, además, se establecerá una comunicación serial entre la placa y una aplicación móvil aprovechando la funcionalidad Bluetooth.

## P-A
```c++
#include <WiFi.h>
#include <WebServer.h>
#include <Arduino.h>

const char* ssid = "MOVISTAR_7CA6"; 
const char* password = "***************"; 
WebServer server(80);

void handle_root() {
    server.send(200, "text/html", "<h1>Hola desde ESP32-S3</h1>");
}

void setup() {
    Serial.begin(115200);
    Serial.println("Try Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected successfully");
    Serial.print("Got IP: ");
    Serial.println(WiFi.localIP());

    server.on("/", handle_root);
    server.begin();
    Serial.println("HTTP server started");
}

void loop() {
    server.handleClient();
}
```
HTML:
```HTML:
String HTML = "<!DOCTYPE html>\
<html>\
<body>\
<h1> Pagina Web creada , Practica 3 - Wifi &#128522;</h1>\
</body>\
</html>";

// Handle root url (/)
void handle_root() {
 server.send(200, "text/html", HTML);
}
```
### Funcionamiento y outputs:

Este código permite configurar la ESP32 como un servidor web que genera una página HTML accesible a través de su IP local. Una vez que la placa se conecta a la red WiFi, se puede acceder a la página web desde un navegador, donde se visualiza el mensaje configurado.

Salida por terminal del código:


Try Connecting to 
MOVISTAR_CF25
............
WiFi connected successfully
Got IP: ***********
HTTP server started


## P-B

```c++
#include <Arduino.h>
#include <BluetoothSerial.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
BluetoothSerial SerialBT;
void setup() {
Serial.begin(115200);
SerialBT.begin("ESP32test"); //Bluetooth device name
Serial.println("The device started, now you can pair it with bluetooth!");
}
void loop() {
if (Serial.available()) {
SerialBT.write(Serial.read());
}
if (SerialBT.available()) {
Serial.write(SerialBT.read());
}
delay(20);
}
```
### Funcionamiento :

Este código establece una conexión Bluetooth utilizando el protocolo clásico Bluetooth (SPP) para permitir la comunicación entre el dispositivo ESP32 y un móvil. Los datos que se envíen desde el móvil se recibirán a través de Bluetooth y se enviarán al puerto serie, y viceversa, permitiendo un intercambio de datos en tiempo real.