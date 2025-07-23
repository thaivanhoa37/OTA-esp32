#ifndef DISPLAY_H
#define DISPLAY_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include "config.h"

class Display {
private:
    Adafruit_SSD1306* display;
    unsigned long lastStatusUpdate;
    unsigned long notificationEndTime;
    bool notificationActive;
    String currentNotification;
    uint8_t brightness;

public:
    Display() {
        display = new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
        lastStatusUpdate = 0;
        notificationActive = false;
        brightness = DEFAULT_BRIGHTNESS;
    }

    bool begin() {
        if(!display->begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
            return false;
        }
        display->clearDisplay();
        display->setTextColor(SSD1306_WHITE);
        display->dim(brightness < BRIGHTNESS_LEVELS/2);
        return true;
    }

    void showNotification(const String& message) {
        currentNotification = message;
        notificationActive = true;
        notificationEndTime = millis() + NOTIFICATION_TIMEOUT;
        
        // Save current display area under notification
        display->fillRect(0, 0, SCREEN_WIDTH, 16, SSD1306_BLACK);
        display->setTextSize(1);
        display->setCursor(2, 4);
        display->print(message);
        display->display();
    }

    void drawStatusBar(const String& wifiStatus, int signalStrength, float cpuTemp) {
        if (millis() - lastStatusUpdate < STATUS_BAR_UPDATE_INTERVAL) {
            return;
        }
        
        display->fillRect(0, 0, SCREEN_WIDTH, 8, SSD1306_BLACK);
        display->setTextSize(1);
        
        // WiFi icon and strength (left side)
        if (wifiStatus != "") {
            // Draw WiFi icon (simplified)
            display->drawPixel(2, 6, SSD1306_WHITE);
            display->drawLine(0, 4, 4, 4, SSD1306_WHITE);
            if (signalStrength > 25) {
                display->drawLine(-1, 2, 5, 2, SSD1306_WHITE);
            }
            if (signalStrength > 50) {
                display->drawLine(-2, 0, 6, 0, SSD1306_WHITE);
            }
        }

        // Uptime in HH:MM (center)
        unsigned long uptime = millis() / 1000; // Convert to seconds
        unsigned int uptimeHours = (uptime / 3600) % 100; // Limit to 2 digits
        unsigned int uptimeMins = (uptime / 60) % 60;
        char timeStr[6];
        sprintf(timeStr, "%02u:%02u", uptimeHours, uptimeMins);
        display->setCursor(SCREEN_WIDTH/2 - 12, 0);
        display->print(timeStr);

        // Temperature (right side)
        display->setCursor(SCREEN_WIDTH - 24, 0);
        display->print(cpuTemp, 0);
        display->print("C");
        
        display->display();
        lastStatusUpdate = millis();
    }

    void drawMenu(const char* title, const char** items, int itemCount, int selectedIndex) {
        display->clearDisplay();
        
        // Draw title
        display->setTextSize(1);
        display->setCursor(0, 0);
        display->print(title);
        display->drawLine(0, 9, SCREEN_WIDTH-1, 9, SSD1306_WHITE);

        // Draw menu items
        for(int i = 0; i < itemCount && i < 6; i++) {
            display->setCursor(2, 12 + i*8);
            if(i == selectedIndex) {
                display->fillRect(0, 11 + i*8, SCREEN_WIDTH, 9, SSD1306_WHITE);
                display->setTextColor(SSD1306_BLACK);
                display->print(items[i]);
                display->setTextColor(SSD1306_WHITE);
            } else {
                display->print(items[i]);
            }
        }
        
        // Draw scrollbar if needed
        if(itemCount > 6) {
            display->drawRect(SCREEN_WIDTH-3, 11, 3, 53, SSD1306_WHITE);
            int scrollHeight = 53 * (6.0/itemCount);
            int scrollPos = 11 + (53-scrollHeight) * (selectedIndex/(itemCount-1.0));
            display->fillRect(SCREEN_WIDTH-3, scrollPos, 3, scrollHeight, SSD1306_WHITE);
        }
        
        display->display();
    }

    void setBrightness(uint8_t level) {
        brightness = level;
        display->dim(brightness < BRIGHTNESS_LEVELS/2);
    }

    void clear() {
        display->clearDisplay();
        display->display();
    }

    ~Display() {
        delete display;
    }
};

#endif
