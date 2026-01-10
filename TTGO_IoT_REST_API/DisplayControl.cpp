#include "DisplayControl.h"

DisplayControl::DisplayControl(TFT_eSPI* tft) {
    _tft = tft;
}

void DisplayControl::begin() {
    _tft->init();
    _tft->setRotation(1);
    _tft->fillScreen(TFT_BLACK);
}

void DisplayControl::clear() {
    _tft->fillScreen(TFT_BLACK);
}

void DisplayControl::showMessage(String title, String msg, uint16_t color) {
    _tft->fillScreen(TFT_BLACK);
    _tft->setTextColor(color);
    _tft->setTextSize(2);
    _tft->setCursor(50, 30);
    _tft->println(title);
    _tft->setTextSize(1);
    _tft->setCursor(50, 60);
    _tft->println(msg);
}

void DisplayControl::showStatus(float temperature, int lightPercent, bool ledState, 
                                bool wifiConnected, bool firebaseReady, String mode) {
    _tft->fillScreen(TFT_BLACK);
    
    // Titre
    _tft->setTextSize(2);
    _tft->setTextColor(TFT_CYAN);
    _tft->setCursor(40, 5);
    _tft->print("TTGO IoT");
    _tft->drawLine(0, 25, 240, 25, TFT_WHITE);
    
    // Température
    _tft->setTextSize(1);
    _tft->setTextColor(TFT_YELLOW);
    _tft->setCursor(10, 30);
    _tft->print("Temp:");
    
    _tft->setTextSize(2);
    if(temperature < 30.0) _tft->setTextColor(TFT_GREEN);
    else if(temperature < 35.0) _tft->setTextColor(TFT_CYAN);
    else if(temperature < 40.0) _tft->setTextColor(TFT_ORANGE);
    else _tft->setTextColor(TFT_RED);
    
    _tft->setCursor(70, 28);
    _tft->print(temperature, 1);
    _tft->print("C");
    
    // Lumière
    _tft->setTextSize(1);
    _tft->setTextColor(TFT_YELLOW);
    _tft->setCursor(10, 52);
    _tft->print("Lum:");
    
    _tft->setTextSize(2);
    if(lightPercent < 30) _tft->setTextColor(TFT_BLUE);
    else if(lightPercent < 70) _tft->setTextColor(TFT_YELLOW);
    else _tft->setTextColor(TFT_ORANGE);
    
    _tft->setCursor(70, 50);
    _tft->print(lightPercent);
    _tft->print("%");
    
    // Barre lumière
    int barWidth = map(lightPercent, 0, 100, 0, 220);
    uint16_t barColor = TFT_YELLOW;
    if(lightPercent < 30) barColor = TFT_BLUE;
    if(lightPercent > 70) barColor = TFT_ORANGE;
    _tft->fillRect(10, 72, barWidth, 8, barColor);
    _tft->drawRect(10, 72, 220, 8, TFT_WHITE);
    
    // Mode
    _tft->setTextSize(1);
    _tft->setCursor(10, 85);
    _tft->setTextColor(TFT_MAGENTA);
    _tft->print("Mode: ");
    _tft->print(mode);
    
    // LED
    if (ledState) {
        _tft->fillCircle(200, 90, 12, TFT_GREEN);
        _tft->setTextColor(TFT_GREEN);
        _tft->setTextSize(1);
        _tft->setCursor(190, 108);
        _tft->print("ON");
    } else {
        _tft->fillCircle(200, 90, 12, TFT_RED);
        _tft->setTextColor(TFT_RED);
        _tft->setTextSize(1);
        _tft->setCursor(187, 108);
        _tft->print("OFF");
    }
    
    // Status
    _tft->setTextSize(1);
    _tft->setCursor(10, 118);
    if (wifiConnected && firebaseReady) {
        _tft->setTextColor(TFT_GREEN);
        _tft->print("WiFi+FB OK");
    } else if (wifiConnected) {
        _tft->setTextColor(TFT_ORANGE);
        _tft->print("WiFi OK");
    } else {
        _tft->setTextColor(TFT_RED);
        _tft->print("Hors ligne");
    }
}
