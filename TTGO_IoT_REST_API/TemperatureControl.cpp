#include "TemperatureControl.h"
#include "config.h"
#include <math.h>

TemperatureControl::TemperatureControl(uint8_t pin) {
    _pin = pin;
    _bufferIndex = 0;
    
    _R0 = R0;
    _R_AT_25C = R_AT_25C;
    _B_COEFFICIENT = B_COEFFICIENT;
    _T0_KELVIN = T0_KELVIN;
    _VREF = VREF;
    _ADC_MAX = ADC_MAX;
    _ESP32_VMAX = ESP32_VMAX;
    
    for(int i = 0; i < 10; i++) {
        _buffer[i] = 0;
    }
}

void TemperatureControl::begin() {
    pinMode(_pin, INPUT);
    analogSetAttenuation(ADC_11db);
    analogSetWidth(12);
    
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
    
    _buffer[_bufferIndex % 10] = analogRead(_pin);
    _bufferIndex++;
    
    int adcValue = getAverage(_buffer, 10);
    
    if (adcValue == 0) return 25.0;
    
    // Conversion ADC → Tension ESP32 (0-3.3V)
    float measuredVoltage = (adcValue / _ADC_MAX) * _ESP32_VMAX;
    
    // Extrapolation → Tension réelle du circuit (0-5V)
    float realVoltage = measuredVoltage * (_VREF / _ESP32_VMAX);
    
    if (realVoltage > _VREF) realVoltage = _VREF;
    if (realVoltage < 0.1) return 25.0;
    
    // Calcul résistance NTC (NTC en BAS du diviseur)
    float Rth = _R0 * (realVoltage / (_VREF - realVoltage));
    
    // Si les valeurs semblent inversées, commentez la ligne ci-dessus et décommentez celle-ci :
    // float Rth = _R0 * ((_VREF - realVoltage) / realVoltage);
    
    if(Rth < 100 || Rth > 200000) return 25.0;
    
    // Équation Steinhart-Hart
    float TempK = 1.0 / ((1.0 / _T0_KELVIN) + (1.0 / _B_COEFFICIENT) * log(Rth / _R_AT_25C));
    float tempC = TempK - 273.15;
    
    if(tempC < -10.0) tempC = 20.0;
    if(tempC > 80.0) tempC = 40.0;
    
    return tempC;
}

int TemperatureControl::getRawADC() {
    return analogRead(_pin);
}
