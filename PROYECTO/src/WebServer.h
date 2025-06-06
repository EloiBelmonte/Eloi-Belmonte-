#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <SPIFFS.h>
#include "SDCard.h"
#include "OLEDDisplay.h"

class WebServer {
public:
    WebServer(const char* ssid, const char* password, SDCard* sdCard, OLEDDisplay* display);
    bool begin();
    void end();
    bool isActive();
    String getLocalIP();
    void showUploadNotification();
    bool shouldShowWebModeDisplay();
    void showUploadProgress(); // Reverted: Static message, no parameters
    void showUploadError(const String& error);

private:
    const char* ssid_;
    const char* password_;
    AsyncWebServer server_;
    SDCard* sdCard_;
    OLEDDisplay* display_;
    String localIP_;
    bool active_;
    unsigned long uploadNotificationTime_;
    bool uploading_;
    unsigned long uploadStartTime_;
    static const unsigned long UPLOAD_TIMEOUT = 60000; // 60 seconds
    void setupRoutes();
};

#endif