#include "TemperatureControl.h"
#include <math.h>

TemperatureControl::TemperatureControl(uint8_t pin) {
    _pin = pin;
    _bufferIndex = 0;
    
    R0 = 10000.0;
    R_AT_25C = 10000.0;
    B_COEFFICIENT = 3950.0;
    T0_KELVIN = 298.15;
    VREF = 5.0;
    ADC_MAX = 4095.0;
    ESP32_VMAX = 3.3;
    
    for(int i = 0; i < 10; i++) {
        _buffer[i] = 0;
    }
}

void TemperatureControl::begin() {
    pinMode(_pin, INPUT);
    analogSetAttenuation(ADC_11db);
    analogSetWidth(12);
    
    // Initialiser le buffer
    for(int i = 0; i < 10; i++) {
        _buffer[i] = analogRead(_pin);
        delay(10);
    }
}

int TemperatureControl::getAverage(int buffer[], int size) {
    long sum = 0;
    for(int i = 0; i < size; i++) {
        sum += buffer[i];
    }
    return sum / size;
}

float TemperatureControl::readTemperature() {
    // Nettoyage
    for(int i = 0; i < 5; i++) {
        analogRead(_pin);
        delay(2);
    }
    
    // Lecture avec buffer
    _buffer[_bufferIndex % 10] = analogRead(_pin);
    _bufferIndex++;
    
    int adcValue = getAverage(_buffer, 10);
    
    if (adcValue == 0) return 25.0;
    
    // Conversion ADC → Voltage
    float measuredVoltage = (adcValue / ADC_MAX) * ESP32_VMAX;
    float realVoltage = measuredVoltage * (VREF / ESP32_VMAX);
    
    if (realVoltage > VREF) realVoltage = VREF;
    if (realVoltage < 0.5) return 25.0;
    
    // Calcul résistance
    float Rth = R0 * (VREF - realVoltage) / realVoltage;
    
    if(Rth < 1000 || Rth > 100000) return 25.0;
    
    // Équation Steinhart-Hart
    float TempK = 1.0 / ((1.0 / T0_KELVIN) + (1.0 / B_COEFFICIENT) * log(Rth / R_AT_25C));
    float tempC = TempK - 273.15;
    
    if(tempC < -10.0) tempC = 20.0;
    if(tempC > 80.0) tempC = 40.0;
    
    return tempC;
}

int TemperatureControl::getRawADC() {
    return analogRead(_pin);
}
