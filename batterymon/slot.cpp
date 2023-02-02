
#include <Arduino.h>
#include "slot.h"

Slot::Slot(char relayPin, char voltagePin, char greenLEDPin, char redLEDPin) : relayPin(relayPin), voltagePin(voltagePin), greenLEDPin(greenLEDPin), redLEDPin(redLEDPin){
  digitalWrite(relayPin, LOW);
  digitalWrite(greenLEDPin, LOW);
  digitalWrite(redLEDPin, LOW);
  pinMode(relayPin, OUTPUT);
  pinMode(greenLEDPin, OUTPUT);
  pinMode(redLEDPin, OUTPUT);
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

void Slot::setGreenLED(boolean isOn){
  digitalWrite(greenLEDPin, isOn ? HIGH : LOW);
}

void Slot::setRedLED(boolean isOn){
  digitalWrite(redLEDPin, isOn ? HIGH : LOW);
}
