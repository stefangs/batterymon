
#include <Arduino.h>
#include "slot.h"
#include "led.h"

Slot::Slot(char relayPin, char voltagePin, char greenLEDPin, char redLEDPin) : relayPin(relayPin), voltagePin(voltagePin), 
  greenLED(greenLEDPin), redLED(redLEDPin){
  digitalWrite(relayPin, LOW);
  pinMode(relayPin, OUTPUT);
  pinMode(voltagePin, INPUT);
}

int Slot::voltage() {
  return (analogRead(voltagePin) * 5000L) / 1024;
}

void Slot::addLoad(){
  digitalWrite(relayPin, HIGH);
}

void Slot::removeLoad(){
  digitalWrite(relayPin, LOW);
}

Led& 
Slot::getRedLED(){
  return redLED;
}

Led& 
Slot::getGreenLED(){
  return greenLED;
}

void
Slot::loop(){
  greenLED.loop();
  redLED.loop();
}
