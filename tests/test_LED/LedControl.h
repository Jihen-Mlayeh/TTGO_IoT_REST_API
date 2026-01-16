#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <Arduino.h>

class LedControl {
private:
    uint8_t _pin;
    bool _state;
    
public:
    LedControl(uint8_t pin);
    void begin();
    void on();
    void off();
    void toggle();
    bool getState();
};

#endif
