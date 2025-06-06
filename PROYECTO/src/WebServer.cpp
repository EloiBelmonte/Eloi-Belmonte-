#include "WebServer.h"

WebServer::WebServer(const char* ssid, const char* password, SDCard* sdCard, OLEDDisplay* display)
    : ssid_(ssid), password_(password), server_(80), sdCard_(sdCard), display_(display), 
      active_(false), uploadNotificationTime_(0), uploading_(false), uploadStartTime_(0) {}

bool WebServer::begin() {
    if (display_) {
        display_->showText("Buscando Wi-Fi...");
    }
    WiFi.disconnect(true);
    delay(1000);
    WiFi.begin(ssid_, password_);
    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 20000) {
        delay(500);
    }
    if (WiFi.status() != WL_CONNECTED) {
        if (display_) {
            display_->showText("Error Wi-Fi");
        }
        return false;
    }
    localIP_ = WiFi.localIP().toString();
    active_ = true;
    setupRoutes();
    server_.begin();
    Serial.println("Servidor web iniciado en IP: " + localIP_);
    return true;
}

void WebServer::end() {
    server_.end();
    WiFi.disconnect();
    active_ = false;
    uploading_ = false;
    uploadNotificationTime_ = 0;
}

bool WebServer::isActive() {
    return active_;
}

String WebServer::getLocalIP() {
    return localIP_;
}

void WebServer::showUploadNotification() {
    if (display_) {
        display_->showUploadNotification();
        uploadNotificationTime_ = millis();
        uploading_ = false;
    }
}

bool WebServer::shouldShowWebModeDisplay() {
    return isActive() && localIP_ != "" && !uploading_ && 
           (millis() - uploadNotificationTime_ >= 5000 || uploadNotificationTime_ == 0);
}

void WebServer::showUploadProgress() {
    if (display_) {
        display_->showText("Subiendo cancion...");
    }
}

void WebServer::showUploadError(const String& error) {
    if (display_) {
        display_->clear();
        display_->showText("Error:\n" + error);
        uploadNotificationTime_ = millis();
        uploading_ = false;
    }
}

void WebServer::setupRoutes() {
    server_.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        String html = "<!DOCTYPE html><html><head>";
        html += "<title>ESP32 Music Player</title>";
        html += "<style>";
        html += "body { font-family: 'Arial', sans-serif; background-color: rgb(130, 229, 233); display: flex; flex-direction: column; align-items: center; justify-content: center; height: 100vh; margin: 0; }";
        html += "h1 { color: #2c3e50; text-shadow: 1px 1px #ecf0f1; }";
        html += ".container { background: white; padding: 20px; border-radius: 10px; box-shadow: 0 0 10px rgba(0,0,0,0.1); text-align: center; }";
        html += "input[type=file], input[type=submit], button { margin: 10px; padding: 10px 20px; font-size: 16px; border: none; border-radius: 5px; cursor: pointer; }";
        html += "input[type=submit], button { background-color: #3498db; color: white; transition: background-color 0.3s; }";
        html += "input[type=submit]:hover, button:hover { background-color: #2980b9; }";
        html += "input[type=file] { border: 1px solid #ccc; }";
        html += "#progress { display: none; margin: 10px; }";
        html += "#status { color: #2c3e50; font-size: 16px; }";
        html += "</style>";
        html += "<script>";
        html += "function updateProgress(uploaded, total) {";
        html += "  var progress = document.getElementById('progress');";
        html += "  progress.style.display = 'block';";
        html += "  progress.value = uploaded;";
        html += "  progress.max = total;";
        html += "  document.getElementById('status').innerText = 'Subiendo: ' + Math.round((uploaded / total) * 100) + '%';";
        html += "}";
        html += "function showError(message) {";
        html += "  document.getElementById('status').innerText = 'Error: ' + message;";
        html += "}";
        html += "function showSuccess() {";
        html += "  document.getElementById('status').innerText = '¡Cancion subida con éxito!';";
        html += "}";
        html += "</script>";
        html += "</head><body>";
        html += "<div class='container'>";
        html += "<h1>Bienvenido</h1>";
        html += "<h2>Reproductor musical ESP</h2>";
        html += "<form method='POST' action='/upload' enctype='multipart/form-data'>";
        html += "<input type='file' name='file' accept='.mp3'><br>";
        html += "<input type='submit' value='Subir cancion'>";
        html += "</form>";
        html += "<progress id='progress'></progress>";
        html += "<div id='status'></div>";
        html += "<button onclick=\"location.href='/refresh'\">Recargar lista</button>";
        html += "<button onclick=\"location.href='/back'\">Volver al menu</button>";
        html += "</div>";
        html += "</body></html>";
        request->send(200, "text/html", html);
    });

    server_.on("/upload", HTTP_POST, [](AsyncWebServerRequest *request) {
        request->send(200);
    }, [this](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
        static File uploadFile;
        if (!index) {
            // Start of upload
            if (!filename.endsWith(".mp3") && !filename.endsWith(".MP3")) {
                showUploadError("Formato no válido");
                request->send(400, "text/plain", "Solo se permiten archivos MP3");
                return;
            }
            Serial.printf("Subiendo archivo: %s\n", filename.c_str());
            uploading_ = true;
            uploadStartTime_ = millis();
            showUploadProgress(); // Show static "Subiendo canción..."
            String filepath = "/" + filename;
            uploadFile = SD.open(filepath, FILE_WRITE);
            if (!uploadFile) {
                Serial.println("Error al abrir el archivo en SD, intentando SPIFFS...");
                if (!SPIFFS.begin(true)) {
                    Serial.println("Error al inicializar SPIFFS");
                    showUploadError("No se pudo inicializar SPIFFS");
                    request->send(500, "text/plain", "Error al inicializar SPIFFS");
                    return;
                }
                uploadFile = SPIFFS.open(filepath, FILE_WRITE);
                if (!uploadFile) {
                    Serial.println("Error al abrir el archivo en SPIFFS");
                    showUploadError("No se pudo abrir el archivo");
                    request->send(500, "text/plain", "Error al abrir el archivo");
                    return;
                }
            }
        }

        // Check for timeout
        if (millis() - uploadStartTime_ > UPLOAD_TIMEOUT) {
            if (uploadFile) {
                uploadFile.close();
            }
            showUploadError("Tiempo de espera");
            request->send(408, "text/plain", "Tiempo de espera excedido");
            uploading_ = false;
            return;
        }

        // Write data
        if (len) {
            if (uploadFile.write(data, len) != len) {
                uploadFile.close();
                showUploadError("Error al escribir");
                request->send(500, "text/plain", "Error al escribir en archivo");
                uploading_ = false;
                return;
            }
        }

        // End of upload
        if (final) {
            Serial.printf("Subida completada: %s, %u bytes\n", filename.c_str(), index + len);
            uploadFile.close();
            sdCard_->loadPlaylist();
            showUploadNotification();
            request->send(200, "text/plain", "Cancion subida con éxito");
        }
    });

    server_.on("/refresh", HTTP_GET, [this](AsyncWebServerRequest *request) {
        sdCard_->loadPlaylist();
        request->send(200, "text/plain", "Lista recargada. Vuelve al menú y selecciona 'Reproducir SD'.");
    });

    server_.on("/back", HTTP_GET, [this](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", "Volviendo al menú...");
        end();
    });
}