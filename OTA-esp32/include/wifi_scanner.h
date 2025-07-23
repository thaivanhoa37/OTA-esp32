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
                String html = "<html><head>"
                    "<title>WiFi Setup</title>"
                    "<meta name='viewport' content='width=device-width, initial-scale=1'>"
                    "<meta http-equiv='refresh' content='10'>"
                    "<style>"
                    "body { font-family: Arial; margin: 20px; background: #f0f0f0; }"
                    ".container { max-width: 400px; margin: 0 auto; background: white; padding: 20px; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }"
                    "select, input[type='password'] { width: 100%; padding: 8px; margin: 8px 0; border: 1px solid #ddd; border-radius: 4px; }"
                    "input[type='submit'] { background: #4CAF50; color: white; padding: 12px; border: none; width: 100%; border-radius: 4px; cursor: pointer; }"
                    "input[type='submit']:hover { background: #45a049; }"
                    ".signal { display: inline-block; width: 20px; }"
                    ".refresh { float: right; text-decoration: none; padding: 5px 10px; background: #eee; border-radius: 4px; }"
                    "h1 { color: #333; margin-bottom: 20px; }"
                    "option { padding: 5px; }"
                    ".status { color: #666; font-size: 0.9em; margin-top: 15px; }"
                    "</style>"
                    "</head><body>"
                    "<div class='container'>"
                    "<h1>WiFi Configuration</h1>"
                    "<a href='/' class='refresh'>🔄 Refresh</a><br><br>"
                    "<form method='POST' action='/connect'>"
                    "SSID: <select name='ssid'>";
                
                int count;
                NetworkInfo* nets = getNetworks(&count);
                for(int i = 0; i < count; i++) {
                    String signalIcon;
                    int rssi = nets[i].rssi;
                    if (rssi >= -50) signalIcon = "▂▄▆█";
                    else if (rssi >= -60) signalIcon = "▂▄▆_";
                    else if (rssi >= -70) signalIcon = "▂▄__";
                    else if (rssi >= -80) signalIcon = "▂___";
                    else signalIcon = "____";

                    html += "<option value='" + nets[i].ssid + "'>" + signalIcon + " " + nets[i].ssid;
                    html += (nets[i].encryption != WIFI_AUTH_OPEN ? " 🔒" : "");
                    html += " (" + String(rssi) + "dBm)</option>";
                }
                
                html += "</select><br><br>";
                html += "Password: <input type='password' name='password' placeholder='Enter password'><br><br>";
                html += "<input type='submit' value='Connect'>";
                html += "</form>"
                    "<div class='status'>"
                    "Found " + String(count) + " networks<br>"
                    "<small>Page will refresh in <span id='countdown'>10</span> seconds</small><br>"
                    "<small>Device will restart after successful connection</small>"
                    "</div>"
                    "<script>"
                    "var count = 10;"
                    "var counter = setInterval(function(){"
                    "count--;"
                    "document.getElementById('countdown').textContent = count;"
                    "if(count <= 0) clearInterval(counter);"
                    "}, 1000);"
                    "</script>"
                    "</div></body></html>";
                apServer->send(200, "text/html", html);
            });
            
            // Handle connection request
            apServer->on("/connect", HTTP_POST, [this]() {
                String ssid = apServer->arg("ssid");
                String password = apServer->arg("password");
                
                String html = "<html><head>"
                    "<title>Connecting...</title>"
                    "<meta name='viewport' content='width=device-width, initial-scale=1'>"
                    "<style>"
                    "body { font-family: Arial; margin: 20px; background: #f0f0f0; }"
                    ".container { max-width: 400px; margin: 0 auto; background: white; padding: 20px; "
                    "border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); text-align: center; }"
                    ".spinner { border: 4px solid #f3f3f3; border-top: 4px solid #3498db; "
                    "border-radius: 50%; width: 40px; height: 40px; margin: 20px auto; "
                    "animation: spin 1s linear infinite; }"
                    "@keyframes spin { 0% { transform: rotate(0deg); } "
                    "100% { transform: rotate(360deg); } }"
                    "</style></head><body><div class='container'>"
                    "<h1>Connecting...</h1>"
                    "<div class='spinner'></div>"
                    "<p>Attempting to connect to:<br><strong>" + ssid + "</strong></p>"
                    "<p>The device will restart if connection is successful.</p>"
                    "<p>Please wait...</p>"
                    "</div></body></html>";
                
                apServer->send(200, "text/html", html);
                    
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
        // In AP mode, we want to force a new scan regardless of interval
        if (!apMode && (millis() - lastScanTime < WIFI_SCAN_INTERVAL)) {
            return false;
        }

        // Delete previous scan results
        if (networkCount > 0) {
            WiFi.scanDelete();
        }

        // Start new scan
        networkCount = WiFi.scanNetworks(true, true); // async=true, show_hidden=true
        delay(100); // Give some time for scan to start

        // Wait for scan completion with timeout
        int timeout = 10; // 5 seconds timeout
        while (networkCount == WIFI_SCAN_RUNNING && timeout > 0) {
            delay(500);
            networkCount = WiFi.scanComplete();
            timeout--;
        }

        if (networkCount == WIFI_SCAN_FAILED) {
            networkCount = 0;
            return false;
        }
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
        
        // Scan for networks in STA mode while AP is active
        scan();
        
        apMode = true;
        setupAPServer();
    }

    // Periodically scan in AP mode
    void updateAPScan() {
        static unsigned long lastForceRescan = 0;
        
        if (apMode) {
            unsigned long currentMillis = millis();
            
            // Force new scan every WIFI_SCAN_INTERVAL
            if (currentMillis - lastForceRescan >= WIFI_SCAN_INTERVAL) {
                lastForceRescan = currentMillis;
                
                // Temporarily disable AP to improve scan
                WiFi.softAPdisconnect(false);
                delay(100);
                
                // Perform scan
                scan();
                
                // Restore AP
                IPAddress apIP(192, 168, AP_IP_OCTET, 1);
                IPAddress gateway(192, 168, AP_IP_OCTET, 1);
                IPAddress subnet(255, 255, 255, 0);
                WiFi.softAPConfig(apIP, gateway, subnet);
                WiFi.softAP(AP_SSID, AP_PASSWORD, AP_CHANNEL, false, AP_MAX_CONNECTIONS);
            }
        }
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
            updateAPScan();  // Keep scanning while in AP mode
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
