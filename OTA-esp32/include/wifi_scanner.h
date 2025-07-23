#ifndef WIFI_SCANNER_H
#define WIFI_SCANNER_H

#include <WiFi.h>
#include <WebServer.h>
#include "config.h"

struct NetworkInfo {
    String ssid;
    int32_t rssi;
    wifi_auth_mode_t encryption;
    bool isConnected;
};

class WiFiScanner {
private:
    NetworkInfo networks[MAX_NETWORKS];
    int networkCount;
    unsigned long lastScanTime;
    String connectedSSID;
    WebServer* apServer;
    bool apMode;

    void setupAPServer() {
        if (!apServer) {
            apServer = new WebServer(80);
            
            // Serve configuration page
            apServer->on("/", HTTP_GET, [this]() {
                String html = "<html><head><title>WiFi Setup</title></head><body>";
                html += "<h1>WiFi Configuration</h1>";
                html += "<form method='POST' action='/connect'>";
                html += "SSID: <select name='ssid'>";
                
                int count;
                NetworkInfo* nets = getNetworks(&count);
                for(int i = 0; i < count; i++) {
                    html += "<option value='" + nets[i].ssid + "'>" + nets[i].ssid;
                    html += (nets[i].encryption != WIFI_AUTH_OPEN ? " 🔒" : "");
                    html += " (" + String(nets[i].rssi) + "dBm)</option>";
                }
                
                html += "</select><br><br>";
                html += "Password: <input type='password' name='password'><br><br>";
                html += "<input type='submit' value='Connect'>";
                html += "</form></body></html>";
                apServer->send(200, "text/html", html);
            });
            
            // Handle connection request
            apServer->on("/connect", HTTP_POST, [this]() {
                String ssid = apServer->arg("ssid");
                String password = apServer->arg("password");
                
                apServer->send(200, "text/html", 
                    "<html><body><h1>Connecting...</h1>"
                    "Attempting to connect to " + ssid + "<br>"
                    "The device will restart in station mode if successful.</body></html>");
                    
                // Try to connect
                if(connect(ssid.c_str(), password.c_str())) {
                    // Save credentials and restart
                    delay(2000);
                    ESP.restart();
                }
            });
            
            apServer->begin();
        }
    }

public:
    WiFiScanner() : 
        networkCount(0), 
        lastScanTime(0), 
        connectedSSID(""), 
        apServer(nullptr),
        apMode(false) {
    }

    ~WiFiScanner() {
        if (apServer) {
            delete apServer;
        }
    }

    bool scan() {
        if (millis() - lastScanTime < WIFI_SCAN_INTERVAL) {
            return false;
        }

        networkCount = WiFi.scanNetworks();
        if (networkCount == 0) {
            return false;
        }

        // Store only up to MAX_NETWORKS
        int count = min(networkCount, MAX_NETWORKS);
        for (int i = 0; i < count; i++) {
            networks[i].ssid = WiFi.SSID(i);
            networks[i].rssi = WiFi.RSSI(i);
            networks[i].encryption = WiFi.encryptionType(i);
            networks[i].isConnected = (networks[i].ssid == connectedSSID);
        }

        lastScanTime = millis();
        return true;
    }

    bool connect(const char* ssid, const char* password) {
        WiFi.begin(ssid, password);
        
        // Wait for connection with timeout
        int attempts = 0;
        while (WiFi.status() != WL_CONNECTED && attempts < 20) {
            delay(500);
            attempts++;
        }

        if (WiFi.status() == WL_CONNECTED) {
            connectedSSID = String(ssid);
            return true;
        }
        return false;
    }

    void disconnect() {
        WiFi.disconnect();
        connectedSSID = "";
    }

    void startAPMode() {
        WiFi.mode(WIFI_AP_STA);
        
        // Configure AP
        IPAddress apIP(192, 168, AP_IP_OCTET, 1);
        IPAddress gateway(192, 168, AP_IP_OCTET, 1);
        IPAddress subnet(255, 255, 255, 0);
        
        WiFi.softAPConfig(apIP, gateway, subnet);
        WiFi.softAP(AP_SSID, AP_PASSWORD, AP_CHANNEL, false, AP_MAX_CONNECTIONS);
        
        apMode = true;
        setupAPServer();
    }

    void stopAPMode() {
        if (apMode) {
            if (apServer) {
                apServer->stop();
                delete apServer;
                apServer = nullptr;
            }
            WiFi.softAPdisconnect(true);
            WiFi.mode(WIFI_STA);
            apMode = false;
        }
    }

    void handleClient() {
        if (apMode && apServer) {
            apServer->handleClient();
        }
    }

    void enableAPMode(bool enable) {
        if (enable && !apMode) {
            startAPMode();
        } else if (!enable && apMode) {
            stopAPMode();
        }
    }

    bool isAPMode() {
        return apMode;
    }

    String getAPSSID() {
        return String(AP_SSID);
    }

    IPAddress getAPIP() {
        return WiFi.softAPIP();
    }

    NetworkInfo* getNetworks(int* count) {
        *count = min(networkCount, MAX_NETWORKS);
        return networks;
    }

    bool isConnected() {
        return WiFi.status() == WL_CONNECTED;
    }

    String getConnectedSSID() {
        return connectedSSID;
    }

    IPAddress getIP() {
        return WiFi.localIP();
    }

    int32_t getSignalStrength() {
        return WiFi.RSSI();
    }
};

#endif
