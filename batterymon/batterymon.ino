
#include "discharger.h"
#include "slot.h"
#include "serialreporter.h"
#include "doublereporter.h"
#include "eepromreporter.h"
#include "exteepromreporter.h"
//#include <AT24C256.h> // dantudose
#include <Wire.h>

Slot slot0(2, A0, 3, 4);
SerialReporter sreporter;
//AT24C256 eeprom(0x50);
ExtEEPromReporter  xereporter;
EEPromReporter  ereporter;
DoubleReporter dreporter(sreporter, ereporter);
Discharger discharger0(slot0, dreporter);

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Serial.println("Starting up");
  xereporter.begin(0x50);
  ereporter.printReport();
}

void loop() {
  discharger0.loop();
}
