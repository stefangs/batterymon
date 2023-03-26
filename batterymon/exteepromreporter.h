
#include "reporter.h"

struct ExtSample;

enum EEPromState {ok, initialized, error};

class ExtEEPromReporter : public Reporter {
  public:
    ExtEEPromReporter();
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
    EEPromState state;
};
