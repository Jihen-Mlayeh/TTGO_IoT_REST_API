#ifndef DISPLAY_CONTROL_H
#define DISPLAY_CONTROL_H

#include <TFT_eSPI.h>
#include <Arduino.h>

class DisplayControl {
private:
    TFT_eSPI* _tft;
    
public:
    DisplayControl(TFT_eSPI* tft);
    void begin();
    void showMessage(String title, String msg, uint16_t color);
    void showStatus(float temperature, int lightPercent, bool ledState, 
                   bool wifiConnected, bool firebaseReady, String mode);
    void clear();
};

#endif
