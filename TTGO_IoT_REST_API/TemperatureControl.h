#ifndef TEMPERATURE_CONTROL_H
#define TEMPERATURE_CONTROL_H

#include <Arduino.h>

class TemperatureControl {
private:
    uint8_t _pin;
    int _buffer[10];
    int _bufferIndex;
    
    float R0;
    float R_AT_25C;
    float B_COEFFICIENT;
    float T0_KELVIN;
    float VREF;
    float ADC_MAX;
    float ESP32_VMAX;
    
    int getAverage(int buffer[], int size);
    
public:
    TemperatureControl(uint8_t pin);
    void begin();
    float readTemperature();
    int getRawADC();
};

#endif
