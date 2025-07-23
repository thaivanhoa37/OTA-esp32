#ifndef CONFIG_H
#define CONFIG_H

// Screen Settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

// Button Pins
#define BUTTON_UP 2
#define BUTTON_DOWN 0
#define BUTTON_SELECT 4

// AP Mode Settings
#define AP_SSID "ESP32-Config"
#define AP_PASSWORD "12345678"
#define AP_CHANNEL 1
#define AP_MAX_CONNECTIONS 1
#define AP_IP_OCTET 1  // Will create IP 192.168.1.1

// WiFi Settings
#define WIFI_SCAN_INTERVAL 10000  // ms
#define MAX_NETWORKS 20

// OTA Settings
#define OTA_PORT 8080
#define OTA_HOSTNAME "esp32-ota"
#define OTA_PASSWORD "admin"

// Display Update Intervals
#define STATUS_BAR_UPDATE_INTERVAL 1000
#define NOTIFICATION_TIMEOUT 3000

// System Settings
#define BRIGHTNESS_LEVELS 4
#define SCREEN_TIMEOUT_OPTIONS {30, 60, 120, 300} // seconds
#define DEFAULT_BRIGHTNESS 2
#define DEFAULT_SCREEN_TIMEOUT 60

#endif
