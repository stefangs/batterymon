
#include <Arduino.h>
#include "slot.h"

Slot::Slot(char relayPin, char voltagePin) : relayPin(relayPin), voltagePin(voltagePin){
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
