#include <Arduino.h>
//#include <AT24C256.h> // dantudose
#include "exteepromreporter.h"
#include "serialreporter.h"

#define MINUTE_IN_MS (60000)
#define SAMPLE_TIME (60000)
#define MARK (0xAABBCCD5)
#define CURRENT_SESSION (20)
#define REPORT_START (100)
#define REPORT_SIZE (4096)
#define SLOTS_PER_REPORT (REPORT_SIZE/sizeof(ExtSample))
#define PAGE_SIZE (64)

struct ExtSample {
  unsigned int loadedVoltage : 11;
  unsigned int unloadedVoltage : 11;
} extSample;

struct ExtHeader {
  unsigned long mark;
  int lastReport;
} header;

ExtEEPromReporter::ExtEEPromReporter(){
}

int 
ExtEEPromReporter::rawWriteEEProm(uint16_t address, const uint8_t* data, size_t len){
	twoWire->beginTransmission(i2cAddress);
	twoWire->write((uint8_t)((address >> 8) & 0xFF));
	twoWire->write((uint8_t)(address & 0xFF));
  int written = twoWire->write(data, len);
	twoWire->endTransmission();
  delay(30); // AT24C256 needs 5-20 ms time after write (tWR Write Cycle Time) to become available
  return written;
}

int 
ExtEEPromReporter::writeEEProm(uint16_t address, const uint8_t* data, size_t len){
  const uint8_t* dataToWrite = data;
  size_t lenRemaining = len;
  uint16_t nextAddress = address;
  int written = 0;
  do {
    uint16_t locationOnPage = nextAddress % PAGE_SIZE;
    size_t maxBytesToWrite = PAGE_SIZE - locationOnPage;
    size_t bytesToWrite = min(maxBytesToWrite, lenRemaining);
    written += rawWriteEEProm(nextAddress, dataToWrite, bytesToWrite);
    lenRemaining -= bytesToWrite;
    dataToWrite += bytesToWrite;
    nextAddress += bytesToWrite;
  } while (lenRemaining > 0);
  return written;
}

int
ExtEEPromReporter::readEEProm(uint16_t address, uint8_t* data, uint8_t len){
	twoWire->beginTransmission(i2cAddress);
	twoWire->write((uint8_t)((address >> 8) & 0xFF));
	twoWire->write((uint8_t)(address & 0xFF));
	twoWire->endTransmission();
	twoWire->requestFrom(i2cAddress, len);
	int byteNumber;
	for(byteNumber = 0; (byteNumber < len) && twoWire->available(); byteNumber++){
		data[byteNumber] = twoWire->read();
	}
  return byteNumber;
}

void 
ExtEEPromReporter::begin(uint8_t address, TwoWire& i2c) {
  i2cAddress = address;
  twoWire = &i2c;
  readEEProm(0, (uint8_t *)&header, sizeof(ExtHeader));
  if (header.mark != MARK) {
    Serial.println("No mark, resetting eeprom");
    header.mark = MARK;
    header.lastReport = CURRENT_SESSION;
    writeEEProm(0, (uint8_t*)&header, sizeof(ExtHeader));
    slot = 0;
    extSample.unloadedVoltage = 0;
    writeSample(&extSample);
  }
  header.mark = 0; // reset to test read  
  readEEProm(0, (uint8_t *)&header, sizeof(ExtHeader));
  if (header.mark != MARK) {
    Serial.println("*** Error, could not read/write to external eeprom ***");
    Serial.println(header.mark,HEX);
  }
}

void
ExtEEPromReporter::writeSample(ExtSample* sample){
  int written = writeEEProm(REPORT_START + slot * sizeof(ExtSample), (uint8_t*)sample, sizeof(ExtSample));
  if (written != sizeof(ExtSample)) {
    Serial.println("Wrong number bytes written to I2C");
  }
}

void
ExtEEPromReporter::readSample(ExtSample* sample){
  int read = readEEProm(REPORT_START + slot * sizeof(ExtSample), (uint8_t*)sample, sizeof(ExtSample));
  if (read != sizeof(ExtSample)) {
    Serial.println("Wrong number bytes read from I2C");
  }
}

void
ExtEEPromReporter::reportResume(int startVoltage) {
  nextSample = millis() + SAMPLE_TIME;
  slot = 0;
  readSample(&extSample);
  // Find next free slot (or the last)
  while ((slot + 2 < SLOTS_PER_REPORT) && extSample.unloadedVoltage != 0) {
    slot++;
    readSample(&extSample);
  }
}

void
ExtEEPromReporter::reportStart(int startVoltage, int loadVoltage, int current) {
  slot = 0;
  nextSample = millis();
  reportSample(0, loadVoltage, startVoltage, 0, 0);
  header.lastReport++;
  writeEEProm(0, (uint8_t*)&header, sizeof(ExtHeader));
}

void
ExtEEPromReporter::reportSample(long unsigned int timeMs, int loadedVoltage, int unloadedVoltage, int current, int mAh) {
  if (abs(millis() - nextSample) < 30000) {
    nextSample += SAMPLE_TIME;
    extSample.loadedVoltage = loadedVoltage;
    extSample.unloadedVoltage = unloadedVoltage;
    writeSample(&extSample);
    if (slot + 2 < SLOTS_PER_REPORT)  {
      slot++;      
    }
    extSample.unloadedVoltage = 0;
    writeSample(&extSample);
  }
}

void
ExtEEPromReporter::reportEnd(int mAh) {
}

void
ExtEEPromReporter::reportWaiting() {
}

void
ExtEEPromReporter::printSample(int session, long unsigned int timeMs, int loadedVoltage, int unloadedVoltage, int current, int mAh) {
  Serial.print(">S,");
  Serial.print(session);
  Serial.print(',');
  Serial.print(timeMs / MINUTE_IN_MS);
  Serial.print(',');
  Serial.print(loadedVoltage);
  Serial.print(',');
  Serial.print(unloadedVoltage);
  Serial.print(',');
  Serial.print(current);
  Serial.print(',');
  Serial.println(mAh);
}

void 
ExtEEPromReporter::printReport() {
  int currentSlot = slot;
  slot = 0;
  long time = 0;
  int maxLoadedVoltage = 0;
  int minLoadedVoltage = 2000;
  int maxUnloadedVoltage = 0;
  int minUnloadedVoltage = 2000;
  int maxCurrent = 0;
  int minCurrent = 2000;
  long mAMinutes = 0;
  double joule = 0;
  
  Serial.println("*Start report*");
  Serial.print(">B,");
  Serial.println(header.lastReport);
  readSample(&extSample);
  while ((extSample.unloadedVoltage != 0) && (slot < SLOTS_PER_REPORT)) {
    int current = extSample.loadedVoltage * 10 / 33;
    maxLoadedVoltage = max(maxLoadedVoltage, extSample.loadedVoltage);
    minLoadedVoltage = min(minLoadedVoltage, extSample.loadedVoltage);
    maxUnloadedVoltage = max(maxUnloadedVoltage, extSample.unloadedVoltage);
    minUnloadedVoltage = min(minUnloadedVoltage, extSample.unloadedVoltage);
    maxCurrent = max(maxCurrent, current);
    minCurrent = min(minCurrent, current);
    mAMinutes += current;
    joule += (extSample.loadedVoltage * (double)current * 60.0)  / 1000000.0;
    printSample(header.lastReport, time, extSample.loadedVoltage, extSample.unloadedVoltage, current, 0);
    time += SAMPLE_TIME;
    slot++;
    readSample(&extSample);
  }
  slot = currentSlot;
  Serial.print("Max loaded voltage: ");
  Serial.print(maxLoadedVoltage);
  Serial.println(" mV");

  Serial.print("Min loaded voltage: ");
  Serial.print(minLoadedVoltage);
  Serial.println(" mV");

  Serial.print("Max unloaded voltage: ");
  Serial.print(maxUnloadedVoltage);
  Serial.println(" mV");

  Serial.print("Min unloaded voltage: ");
  Serial.print(minUnloadedVoltage);
  Serial.println(" mV");

  Serial.print("Max current: ");
  Serial.print(maxCurrent);
  Serial.println(" mA");
  
  Serial.print("Min current: ");
  Serial.print(minCurrent);
  Serial.println(" mA");

  Serial.print("Charge: ");
  Serial.print(mAMinutes / 60);
  Serial.println(" mAh");

  Serial.print("Energy: ");
  Serial.print(joule);
  Serial.println(" J");
  Serial.print(">E,");
  Serial.println(header.lastReport);
  Serial.println("*End report*");
}
