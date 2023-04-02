
#include "discharger.h"
#include "slot.h"
#include "serialreporter.h"
#include "doublereporter.h"
#include "eepromreporter.h"
#include "exteepromreporter.h"
#include <Wire.h>

Slot slot0(2, A0, 3, 4);
SerialReporter sreporter;
ExtEEPromReporter  xereporter;
DoubleReporter dreporter(sreporter, xereporter);
Discharger discharger0(slot0, dreporter);

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Serial.println("Starting up");
  xereporter.begin(0x50);
  xereporter.printReport();
}

void loop() {
  discharger0.loop();
}
