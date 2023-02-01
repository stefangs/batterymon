#include "reporter.h"

class DoubleReporter : public Reporter {
  public:
    DoubleReporter(Reporter& reporter1, Reporter& reporter2);
    virtual void reportStart(int startVoltage, int loadVoltage, int current);
    virtual void reportSample(long unsigned int timeMs, int loadedVoltage, int unloadedVoltage, int current, int mAh);
    virtual void reportEnd(int mAh);
    virtual void reportWaiting();

  private:
    Reporter& reporter1;
    Reporter& reporter2;
};
