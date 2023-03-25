
#include "reporter.h"

struct ExtSample;

class ExtEEPromReporter : public Reporter {
  public:
    ExtEEPromReporter(byte address);
    void reportResume(int startVoltage);
    void reportStart(int startVoltage, int loadVoltage, int current);
    void reportSample(long unsigned int timeMs, int loadedVoltage, int unloadedVoltage, int current, int mAh);
    void reportEnd(int mAh);
    void reportWaiting();
    void printReport();
  private:
    void writeSample(ExtSample* sample);
    void readSample(ExtSample* sample);
    void readSample
    int slot;
    long nextSample;
};
