
#include "Wire.h"
#include "reporter.h"

struct ExtSample;
class AT24C256;

class ExtEEPromReporter : public Reporter {
  public:
    ExtEEPromReporter();
    void begin(uint8_t address, TwoWire& i2c = Wire);
    void reportResume(int startVoltage);
    void reportStart(int startVoltage, int loadVoltage, int current);
    void reportSample(long unsigned int timeMs, int loadedVoltage, int unloadedVoltage, int current, int mAh);
    void reportEnd(int mAh);
    void reportWaiting();
    void printReport();
  private:
    void writeSample(ExtSample* sample);
    void readSample(ExtSample* sample);
    int slot;
    long nextSample;
    void writeEEProm(uint16_t writeAddress, uint8_t* data, size_t len);
    void readEEProm(uint16_t readAddress, uint8_t* data, size_t len);
    uint8_t i2cAddress;
    TwoWire* twoWire;
};
