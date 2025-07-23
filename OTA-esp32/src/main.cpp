#include <Arduino.h>
#include <Wire.h>
#include <ESPmDNS.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <Update.h>
#include "config.h"
#include "display.h"
#include "wifi_scanner.h"
#include "menu.h"

// Global objects
Display* display;
WiFiScanner* wifiScanner;
Menu* menu;
WebServer server(OTA_PORT);

// Button states
volatile bool upPressed = false;
volatile bool downPressed = false;
volatile bool selectPressed = false;

// Button debounce
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

void IRAM_ATTR handleUpButton() {
    if((millis() - lastDebounceTime) > debounceDelay) {
        upPressed = true;
        lastDebounceTime = millis();
    }
}

void IRAM_ATTR handleDownButton() {
    if((millis() - lastDebounceTime) > debounceDelay) {
        downPressed = true;
        lastDebounceTime = millis();
    }
}

void IRAM_ATTR handleSelectButton() {
    if((millis() - lastDebounceTime) > debounceDelay) {
        selectPressed = true;
        lastDebounceTime = millis();
    }
}

void setupOTA() {
    // OTA Update webpage
    server.on("/", HTTP_GET, []() {
        server.sendHeader("Connection", "close");
        server.send(200, "text/html", 
            "<form method='POST' action='/update' enctype='multipart/form-data'>"
            "<input type='file' name='update'>"
            "<input type='submit' value='Update'>"
            "</form>");
    });

    server.on("/update", HTTP_POST, []() {
        server.sendHeader("Connection", "close");
        server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
        ESP.restart();
    }, []() {
        HTTPUpload& upload = server.upload();
        if(upload.status == UPLOAD_FILE_START) {
            Serial.printf("Update: %s\n", upload.filename.c_str());
            if(!Update.begin(UPDATE_SIZE_UNKNOWN)) {
                Update.printError(Serial);
            }
        } else if(upload.status == UPLOAD_FILE_WRITE) {
            if(Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
                Update.printError(Serial);
            }
        } else if(upload.status == UPLOAD_FILE_END) {
            if(Update.end(true)) {
                Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
            } else {
                Update.printError(Serial);
            }
        }
    });

    server.begin();
}

void setup() {
    Serial.begin(115200);
    Wire.begin();
    
    // Initialize display
    display = new Display();
    if (!display->begin()) {
        Serial.println("Display initialization failed!");
        while(1);
    }
    display->showNotification("Starting...");

    // Initialize WiFi
    wifiScanner = new WiFiScanner();
    WiFi.mode(WIFI_STA);
    
    // Initialize menu system
    menu = new Menu(display, wifiScanner);
    
    // Setup button pins with interrupts
    pinMode(BUTTON_UP, INPUT_PULLUP);
    pinMode(BUTTON_DOWN, INPUT_PULLUP);
    pinMode(BUTTON_SELECT, INPUT_PULLUP);
    
    attachInterrupt(BUTTON_UP, handleUpButton, FALLING);
    attachInterrupt(BUTTON_DOWN, handleDownButton, FALLING);
    attachInterrupt(BUTTON_SELECT, handleSelectButton, FALLING);
    
    // Initialize MDNS for OTA - only after WiFi is connected
    display->showNotification("Connect to WiFi first");
    
    setupOTA();
    
    // Show main menu
    menu->drawMainMenu();
}

void loop() {
    static bool mdnsStarted = false;
    
    // Handle button presses
    if(upPressed) {
        menu->handleUpButton();
        upPressed = false;
    }
    if(downPressed) {
        menu->handleDownButton();
        downPressed = false;
    }
    if(selectPressed) {
        menu->handleSelectButton();
        selectPressed = false;
    }
    
    // Start mDNS once WiFi is connected
    if (WiFi.status() == WL_CONNECTED && !mdnsStarted) {
        if (MDNS.begin(OTA_HOSTNAME)) {
            display->showNotification("OTA Ready");
            mdnsStarted = true;
            setupOTA();
        }
    }
    
    // Handle web servers
    if (WiFi.status() == WL_CONNECTED) {
        server.handleClient();  // Handle OTA server
    }
    wifiScanner->handleClient();  // Handle AP mode server if active
    
    // Regular menu updates (status bar, etc)
    menu->update();
    
    // Small delay to prevent excessive CPU usage
    delay(10);
}
