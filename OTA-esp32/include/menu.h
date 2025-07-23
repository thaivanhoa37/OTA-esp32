#ifndef MENU_H
#define MENU_H

#include <functional>
#include "config.h"
#include "display.h"
#include "wifi_scanner.h"

enum MenuState {
    MAIN_MENU,
    WIFI_SCAN_MENU,
    WIFI_STATUS_MENU,
    OTA_UPDATE_MENU,
    SYSTEM_INFO_MENU,
    SETTINGS_MENU
};

class Menu {
private:
    Display* display;
    WiFiScanner* wifiScanner;
    MenuState currentState;
    int selectedIndex;
    
    const char* mainMenuItems[6] = {
        "Scan WiFi",
        "WiFi Status",
        "AP Mode",
        "OTA Update",
        "System Info",
        "Settings"
    };

    const char* settingsItems[6] = {
        "Screen Brightness",
        "Screen Timeout",
        "Auto WiFi Connect",
        "Device Name",
        "OTA Password",
        "Factory Reset"
    };

public:
    Menu(Display* disp, WiFiScanner* scanner) {
        display = disp;
        wifiScanner = scanner;
        currentState = MAIN_MENU;
        selectedIndex = 0;
    }

    void handleUpButton() {
        switch(currentState) {
            case MAIN_MENU:
                selectedIndex = (selectedIndex > 0) ? selectedIndex - 1 : 5;
                drawMainMenu();
                break;
            case WIFI_SCAN_MENU:
                if(selectedIndex > 0) {
                    selectedIndex--;
                    startWiFiScan();  // Refresh display with new selection
                }
                break;
            case SETTINGS_MENU:
                selectedIndex = (selectedIndex > 0) ? selectedIndex - 1 : 5;
                drawSettingsMenu();
                break;
            default:
                break;
        }
    }

    void handleDownButton() {
        switch(currentState) {
            case MAIN_MENU:
                selectedIndex = (selectedIndex < 5) ? selectedIndex + 1 : 0;
                drawMainMenu();
                break;
            case WIFI_SCAN_MENU:
                {
                    int count;
                    NetworkInfo* networks = wifiScanner->getNetworks(&count);
                    if(selectedIndex < count - 1) {
                        selectedIndex++;
                        startWiFiScan();  // Refresh display with new selection
                    }
                }
                break;
            case SETTINGS_MENU:
                selectedIndex = (selectedIndex < 5) ? selectedIndex + 1 : 0;
                drawSettingsMenu();
                break;
            default:
                break;
        }
    }

    void handleSelectButton() {
        switch(currentState) {
            case MAIN_MENU:
                switch(selectedIndex) {
                    case 0:
                        currentState = WIFI_SCAN_MENU;
                        selectedIndex = 0;
                        startWiFiScan();
                        break;
                    case 1:
                        currentState = WIFI_STATUS_MENU;
                        showWiFiStatus();
                        break;
                    case 2:
                        toggleAPMode();
                        break;
                    case 3:
                        currentState = OTA_UPDATE_MENU;
                        showOTAMenu();
                        break;
                    case 4:
                        currentState = SYSTEM_INFO_MENU;
                        showSystemInfo();
                        break;
                    case 5:
                        currentState = SETTINGS_MENU;
                        selectedIndex = 0;
                        drawSettingsMenu();
                        break;
                }
                break;
                
            case WIFI_SCAN_MENU:
                {
                    int count;
                    NetworkInfo* networks = wifiScanner->getNetworks(&count);
                    if(selectedIndex < count) {
                        NetworkInfo& selected = networks[selectedIndex];
                        
                        if(selected.encryption != WIFI_AUTH_OPEN) {
                            // For secured networks, show AP mode for web config
                            display->showNotification("Use AP mode to\nconnect to\nsecured networks");
                            delay(2000);
                        } else {
                            // For open networks, connect directly
                            display->showNotification("Connecting to\n" + selected.ssid);
                            if(wifiScanner->connect(selected.ssid.c_str(), "")) {
                                display->showNotification("Connected!");
                            } else {
                                display->showNotification("Connection failed");
                            }
                            delay(2000);
                        }
                        currentState = MAIN_MENU;
                        selectedIndex = 0;
                        drawMainMenu();
                    }
                }
                break;
                
            case SETTINGS_MENU:
                handleSettingsSelection();
                break;
                
            default:
                currentState = MAIN_MENU;
                selectedIndex = 0;
                drawMainMenu();
                break;
        }
    }

    void startWiFiScan() {
        display->showNotification("Scanning WiFi...");
        if (wifiScanner->scan()) {
            int count;
            NetworkInfo* networks = wifiScanner->getNetworks(&count);
            
            // Create network list for display
            const char* networkItems[MAX_NETWORKS];
            char networkLabels[MAX_NETWORKS][32];  // Buffer for network names with signal strength
            
            for(int i = 0; i < count; i++) {
                // Format: SSID [sig] (🔒)
                String label = networks[i].ssid;
                if (networks[i].isConnected) {
                    label += " ✓";
                }
                label += " [" + String(networks[i].rssi) + "dBm]";
                if (networks[i].encryption != WIFI_AUTH_OPEN) {
                    label += " 🔒";
                }
                strncpy(networkLabels[i], label.c_str(), 31);
                networkLabels[i][31] = '\0';  // Ensure null termination
                networkItems[i] = networkLabels[i];
            }
            
            display->drawMenu("WiFi Networks", networkItems, count, selectedIndex);
        } else {
            display->showNotification("No networks found");
            delay(2000);
            currentState = MAIN_MENU;
            drawMainMenu();
        }
    }

    void toggleAPMode() {
        if (!wifiScanner->isAPMode()) {
            wifiScanner->enableAPMode(true);
            String apInfo = "AP Mode Active\nSSID: " + wifiScanner->getAPSSID() + "\n";
            apInfo += "IP: " + wifiScanner->getAPIP().toString() + "\n";
            apInfo += "Pass: " + String(AP_PASSWORD);
            display->showNotification(apInfo);
        } else {
            wifiScanner->enableAPMode(false);
            display->showNotification("AP Mode Disabled");
        }
    }

    void showWiFiStatus() {
        const char* statusItems[6];
        char statusLabels[6][32];
        int itemCount = 0;

        // Status header
        sprintf(statusLabels[itemCount], "Status: %s", 
            wifiScanner->isConnected() ? "Connected" : "Disconnected");
        statusItems[itemCount++] = statusLabels[itemCount];

        if (wifiScanner->isConnected()) {
            // SSID
            sprintf(statusLabels[itemCount], "SSID: %s", 
                wifiScanner->getConnectedSSID().c_str());
            statusItems[itemCount++] = statusLabels[itemCount];
            
            // IP Address
            sprintf(statusLabels[itemCount], "IP: %s", 
                wifiScanner->getIP().toString().c_str());
            statusItems[itemCount++] = statusLabels[itemCount];
            
            // Signal Strength
            sprintf(statusLabels[itemCount], "Signal: %d dBm", 
                wifiScanner->getSignalStrength());
            statusItems[itemCount++] = statusLabels[itemCount];
            
            // MAC Address
            sprintf(statusLabels[itemCount], "MAC: %s", 
                WiFi.macAddress().c_str());
            statusItems[itemCount++] = statusLabels[itemCount];
            
            // Channel
            sprintf(statusLabels[itemCount], "Channel: %d", 
                WiFi.channel());
            statusItems[itemCount++] = statusLabels[itemCount];
        }

        display->drawMenu("WiFi Status", statusItems, itemCount, -1);  // -1 for no selection
    }

    void showOTAMenu() {
        const char* otaItems[5];
        char otaLabels[5][32];
        int itemCount = 0;

        if (wifiScanner->isConnected()) {
            // Status
            sprintf(otaLabels[itemCount], "Status: Ready");
            otaItems[itemCount++] = otaLabels[itemCount];
            
            // IP Address
            sprintf(otaLabels[itemCount], "IP: %s", 
                wifiScanner->getIP().toString().c_str());
            otaItems[itemCount++] = otaLabels[itemCount];
            
            // Port
            sprintf(otaLabels[itemCount], "Port: %d", OTA_PORT);
            otaItems[itemCount++] = otaLabels[itemCount];
            
            // Hostname
            sprintf(otaLabels[itemCount], "Host: %s.local", OTA_HOSTNAME);
            otaItems[itemCount++] = otaLabels[itemCount];
            
            // Instructions
            sprintf(otaLabels[itemCount], "Visit: http://%s:%d", 
                wifiScanner->getIP().toString().c_str(), OTA_PORT);
            otaItems[itemCount++] = otaLabels[itemCount];
            
            display->drawMenu("OTA Update", otaItems, itemCount, -1);
        } else {
            display->showNotification("WiFi not connected");
            delay(2000);
            currentState = MAIN_MENU;
            drawMainMenu();
        }
    }

    void showSystemInfo() {
        const char* infoItems[6];
        char infoLabels[6][32];
        int itemCount = 0;

        // Uptime
        unsigned long uptime = millis() / 1000;
        unsigned int days = uptime / 86400;
        unsigned int hours = (uptime % 86400) / 3600;
        unsigned int mins = (uptime % 3600) / 60;
        sprintf(infoLabels[itemCount], "Uptime: %ud %uh %um", days, hours, mins);
        infoItems[itemCount++] = infoLabels[itemCount];

        // CPU Temperature
        sprintf(infoLabels[itemCount], "CPU Temp: %.1fC", temperatureRead());
        infoItems[itemCount++] = infoLabels[itemCount];

        // Free Heap
        sprintf(infoLabels[itemCount], "Free RAM: %u KB", ESP.getFreeHeap() / 1024);
        infoItems[itemCount++] = infoLabels[itemCount];

        // Flash Size
        sprintf(infoLabels[itemCount], "Flash: %u MB", ESP.getFlashChipSize() / (1024 * 1024));
        infoItems[itemCount++] = infoLabels[itemCount];

        // CPU Frequency
        sprintf(infoLabels[itemCount], "CPU Freq: %u MHz", ESP.getCpuFreqMHz());
        infoItems[itemCount++] = infoLabels[itemCount];

        // SDK Version
        sprintf(infoLabels[itemCount], "SDK: %s", ESP.getSdkVersion());
        infoItems[itemCount++] = infoLabels[itemCount];

        display->drawMenu("System Info", infoItems, itemCount, -1);  // -1 for no selection
    }

    void drawMainMenu() {
        display->drawMenu("Main Menu", mainMenuItems, 6, selectedIndex);
    }

    void drawSettingsMenu() {
        display->drawMenu("Settings", settingsItems, 6, selectedIndex);
    }

    void handleSettingsSelection() {
        switch(selectedIndex) {
            case 0: // Screen Brightness
                {
                    static uint8_t brightness = DEFAULT_BRIGHTNESS;
                    brightness = (brightness + 1) % BRIGHTNESS_LEVELS;
                    display->setBrightness(brightness);
                    char msg[32];
                    sprintf(msg, "Brightness: %d/%d", brightness + 1, BRIGHTNESS_LEVELS);
                    display->showNotification(msg);
                }
                break;
                
            case 1: // Screen Timeout
                {
                    static const int timeouts[] = SCREEN_TIMEOUT_OPTIONS;
                    static int timeoutIndex = 0;
                    timeoutIndex = (timeoutIndex + 1) % (sizeof(timeouts)/sizeof(timeouts[0]));
                    char msg[32];
                    sprintf(msg, "Timeout: %ds", timeouts[timeoutIndex]);
                    display->showNotification(msg);
                }
                break;
                
            case 2: // Auto WiFi Connect
                {
                    static bool autoConnect = true;
                    autoConnect = !autoConnect;
                    WiFi.setAutoReconnect(autoConnect);
                    display->showNotification(autoConnect ? 
                        "Auto Connect: ON" : "Auto Connect: OFF");
                }
                break;
                
            case 3: // Device Name
                {
                    String newName = "ESP32-" + WiFi.macAddress().substring(9);
                    WiFi.setHostname(newName.c_str());
                    display->showNotification("Name: " + newName);
                }
                break;
                
            case 4: // OTA Password
                {
                    static bool otaAuth = false;
                    otaAuth = !otaAuth;
                    // Update.setPassword(otaAuth ? OTA_PASSWORD : nullptr);
                    display->showNotification(otaAuth ? 
                        "OTA Auth: ON" : "OTA Auth: OFF");
                }
                break;
                
            case 5: // Factory Reset
                {
                    display->showNotification("Resetting...");
                    delay(1000);
                    WiFi.disconnect(true);  // Clear stored credentials
                    ESP.restart();
                }
                break;
        }
        
        delay(1000);  // Show notification
        drawSettingsMenu();
    }

    void update() {
        // Regular updates like status bar, notifications, etc.
        if (wifiScanner->isConnected()) {
            display->drawStatusBar(
                wifiScanner->getConnectedSSID(),
                wifiScanner->getSignalStrength(),
                temperatureRead()
            );
        }
    }

private:
    float temperatureRead() {
        #ifdef ESP32
            // Using ESP32's ADC for a rough temperature reading
            float voltage = analogRead(36) * 3.3 / 4095.0;
            return (voltage - 0.5) * 100.0;  // Approximate conversion
        #else
            return 25.0;  // Default room temperature for unsupported devices
        #endif
    }
};

#endif
