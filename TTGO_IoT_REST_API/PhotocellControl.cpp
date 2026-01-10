#include "PhotocellControl.h"

PhotocellControl::PhotocellControl(uint8_t pin) {
    _pin = pin;
    _bufferIndex = 0;
    
    for(int i = 0; i < 10; i++) {
        _buffer[i] = 0;
    }
}

void PhotocellControl::begin() {
    pinMode(_pin, INPUT);
    
    // Initialiser buffer
    for(int i = 0; i < 10; i++) {
        _buffer[i] = analogRead(_pin);
        delay(10);
    }
}

int PhotocellControl::getAverage(int buffer[], int size) {
    long sum = 0;
    for(int i = 0; i < size; i++) {
        sum += buffer[i];
    }
    return sum / size;
}

int PhotocellControl::readValue() {
    // Nettoyage
    for(int i = 0; i < 5; i++) {
        analogRead(_pin);
        delay(2);
    }
    
    // Lecture avec buffer
    _buffer[_bufferIndex % 10] = analogRead(_pin);
    _bufferIndex++;
    
    return getAverage(_buffer, 10);
}

int PhotocellControl::readPercent() {
    int raw = readValue();
    return map(raw, 0, 4095, 0, 100);
}
