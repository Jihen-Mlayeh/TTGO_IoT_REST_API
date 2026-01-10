#include "LedControl.h"

LedControl::LedControl(uint8_t pin) {
    _pin = pin;
    _state = false;
}

void LedControl::begin() {
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
}

void LedControl::on() {
    _state = true;
    digitalWrite(_pin, HIGH);
}

void LedControl::off() {
    _state = false;
    digitalWrite(_pin, LOW);
}

void LedControl::toggle() {
    _state = !_state;
    digitalWrite(_pin, _state ? HIGH : LOW);
}

bool LedControl::getState() {
    return _state;
}
