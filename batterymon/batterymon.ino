
#include "discharger.h"
#include "slot.h"
#include "serialreporter.h"
#include "doublereporter.h"
#include "eepromreporter.h"
#include "exteepromreporter.h"
#include "commandhandler.h"
#include <Wire.h>

Slot slot0(10, A0, 11, 12);
SerialReporter sreporter;
ExtEEPromReporter  xereporter;
DoubleReporter dreporter(sreporter, xereporter);
Discharger discharger0(slot0, dreporter);
CommandHandler commands(&xereporter);

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Serial.println("Starting up");
  xereporter.begin(0x50);
  //xereporter.printReport();
}

void loop() {
  discharger0.loop();
  commands.loop();
}
