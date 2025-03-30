# Práctica 6 PD ALVARO RAMO IRURRE (COMPAÑERO : ELOI BELMONTE)

Este código se encarga de interactuar con una tarjeta SD usando el bus SPI en el ESP32. Se realiza una escritura y una lectura de archivo en la tarjeta.

## Ej 1 - SD

```c++

#include <SPI.h>
#include <SD.h>

File myFile;

void setup() {
    Serial.begin(115200);
    Serial.print("Iniciando SD...");

    if (!SD.begin(10)) {  // CS en GPIO 10
        Serial.println("No se pudo inicializar");
        return;
    }
    Serial.println("Inicialización exitosa");

    myFile = SD.open("/archivo.txt", FILE_WRITE);
    if (myFile) {
        myFile.println("Hola desde ESP32-S3");
        myFile.close();
        Serial.println("Escritura exitosa");
    } else {
        Serial.println("Error al abrir el archivo");
    }

    myFile = SD.open("/archivo.txt");
    if (myFile) {
        Serial.println("Contenido del archivo:");
        while (myFile.available()) {
            Serial.write(myFile.read());
        }
        myFile.close();
    } else {
        Serial.println("Error al leer el archivo");
    }
}

void loop() {
}

```
### Funcionamiento y output :

Inicialización de la SD: Se intenta iniciar la tarjeta SD con el pin 10 (CS). Si no se puede inicializar, muestra un mensaje de error. Si la inicialización es exitosa, lo indica por el monitor serial.

Escritura en la tarjeta: Se abre un archivo llamado "archivo.txt" y se escribe en él una línea de texto. Si la escritura es exitosa, se cierra el archivo y se muestra el mensaje correspondiente.

Lectura del archivo: Después, se vuelve a abrir el archivo en modo lectura. Si el archivo se abre correctamente, el contenido se imprime en el monitor serial.

Resultado:
Cuando el código se ejecuta correctamente, se crea y guarda un archivo en la tarjeta SD. Además, el contenido del archivo se imprime en el monitor serial, como se muestra en la salida adjunta.


## Ej 2 - RFID

```c++
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 8    // Pin de reset del RC522
#define SS_PIN  9    // Pin SS (SDA) del RC522

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Crear objeto RFID

void setup() {
    Serial.begin(115200);  // Iniciar la comunicación serial
    SPI.begin();           // Iniciar el bus SPI
    mfrc522.PCD_Init();    // Iniciar el módulo RFID
    Serial.println("Escaneando tarjetas RFID...");
}

void loop() {
    // Verificar si hay una nueva tarjeta presente
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
        Serial.print("UID de tarjeta: ");

        // Leer el UID de la tarjeta y mostrarlo en hexadecimal
        for (byte i = 0; i < mfrc522.uid.size; i++) {
            Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
            Serial.print(mfrc522.uid.uidByte[i], HEX);
        }
        Serial.println();

        mfrc522.PICC_HaltA();  // Detener la lectura de la tarjeta
    }
}

```
### Funcionamiento y output :
Inicialización del lector RFID: Se configura la comunicación SPI y se inicializa el módulo MFRC522.

Lectura del UID: En el bucle principal, se verifica si hay una tarjeta RFID cerca. Si es así, se lee su UID y se imprime en formato hexadecimal en el monitor serial.

Detener la lectura: Después de leer el UID, se detiene la lectura de la tarjeta con PICC_HaltA().

Resultado:
Cada vez que una tarjeta RFID se acerca al lector, su UID se muestra en el monitor serial. Este proceso se repite continuamente mientras se encuentren tarjetas cerca del lector.

### Ej extra :

Lamentablemente, no se pudo realizar el ejercicio extra debido a la falta de los materiales del laboratorio en casa. Sin embargo, se tenía planeado realizarlo con los componentes necesarios.

### Fotos del laboratorio :

SD :

https://drive.google.com/file/d/1daK6trmS65C2E6DkC6ERnEVdtxt2rqdb/view?usp=drive_link

RFID : 

https://drive.google.com/file/d/11TUfBO8X6p21YbG-cjpEgxazEo_Ffodg/view?usp=drive_link