
#include <Arduino.h>
#include "led.h"

Led::Led(char pin) : pin(pin), periodMs(0) {
  digitalWrite(pin, LOW);
  pinMode(pin, OUTPUT);
}

void
Led::on() {
  digitalWrite(pin, HIGH);
}

void
Led::off() {
  digitalWrite(pin, LOW);
  periodMs = 0;
}

void 
Led::blinkOn(int periodMs, char dutyPercentage) {
  this->periodMs = periodMs;
  this->dutyPercentage = dutyPercentage;
  loop();
}

void 
Led::blinkOff(){
  periodMs = 0;
}

void 
Led::blinkCount(char count) {
  this->count = count;
}

inline bool 
Led::isBlinkCountOn() {
  if (count == 0) {
    return false;
  }
  return (millis() % (periodMs * 8)) > periodMs * count;
}

void 
Led::loop() {
  if (periodMs > 0) {
    int DutyCycleCutoff = (periodMs * dutyPercentage) / 100;
    if (((millis() % periodMs) < DutyCycleCutoff) || isBlinkCountOn()) {
      digitalWrite(pin, HIGH);
    } else {
      digitalWrite(pin, LOW);
    }
  }
}
