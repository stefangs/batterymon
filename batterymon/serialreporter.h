
#include "reporter.h"

class SerialReporter : public Reporter {
  public:
    virtual void reportResume(int startVoltage);
    virtual void reportStart(int startVoltage, int loadVoltage, int current);
    virtual void reportSample(long unsigned int timeMs, int loadedVoltage, int unloadedVoltage, int current, int mAh);
    virtual void reportEnd(int mAh);
    virtual void reportWaiting();
};
