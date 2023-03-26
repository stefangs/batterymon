
#include "reporter.h"

struct ExtSample;
class AT24C256;

enum EEPromState {ok, initialized, error};

class ExtEEPromReporter : public Reporter {
  public:
    ExtEEPromReporter(AT24C256& eeprom);
    void begin();
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
    AT24C256& eeprom;
    EEPromState state;
};
