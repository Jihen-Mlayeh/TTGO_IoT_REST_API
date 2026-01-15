#ifndef PHOTOCELL_CONTROL_H
#define PHOTOCELL_CONTROL_H

#include <Arduino.h>

class PhotocellControl {
private:
    uint8_t _pin;
    int _buffer[10];
    int _bufferIndex;
    
    int getAverage(int buffer[], int size);
    
public:
    PhotocellControl(uint8_t pin);
    void begin();
    int readValue();
    int readPercent();
};

#endif
