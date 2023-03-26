
#include "discharger.h"
#include "slot.h"
#include "serialreporter.h"
#include "doublereporter.h"
#include "eepromreporter.h"
#include "exteepromreporter.h"
#include <AT24C256.h> // dantudose

Slot slot0(2, A0, 3, 4);
SerialReporter sreporter;
AT24C256 eeprom(0x50);
ExtEEPromReporter  ereporter(eeprom);
//EEPromReporter  ereporter;
DoubleReporter dreporter(sreporter, ereporter);
Discharger discharger0(slot0, dreporter);

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("Starting up");
  ereporter.begin();
  // ExtEEPromReporter  xereporter;  
  ereporter.printReport();
}

void loop() {
  discharger0.loop();
}
