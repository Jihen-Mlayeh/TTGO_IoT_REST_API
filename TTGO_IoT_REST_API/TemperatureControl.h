#ifndef TEMPERATURE_CONTROL_H
#define TEMPERATURE_CONTROL_H

#include <Arduino.h>

class TemperatureControl {
private:
    uint8_t _pin;
    int _buffer[10];
    int _bufferIndex;
    
    float _R0;
    float _R_AT_25C;
    float _B_COEFFICIENT;
    float _T0_KELVIN;
    float _VREF;
    float _ADC_MAX;
    float _ESP32_VMAX;
    
    int getAverage(int buffer[], int size);
    
public:
    TemperatureControl(uint8_t pin);
    void begin();
    float readTemperature();
    int getRawADC();
};

#endif
