
#include "doublereporter.h"

DoubleReporter::DoubleReporter(Reporter& reporter1, Reporter& reporter2) : reporter1(reporter1), reporter2(reporter2){ 
}


void 
DoubleReporter::reportResume(int startVoltage) {
  reporter1.reportResume(startVoltage);
  reporter2.reportResume(startVoltage);
}

void
DoubleReporter::reportStart(int startVoltage, int loadVoltage, int current) {
  reporter1.reportStart(startVoltage, loadVoltage, current);
  reporter2.reportStart(startVoltage, loadVoltage, current);
}

void
DoubleReporter::reportSample(long unsigned int timeMs, int loadedVoltage, int unloadedVoltage, int current, int mAh) {
  reporter1.reportSample(timeMs, loadedVoltage, unloadedVoltage, current, mAh);
  reporter2.reportSample(timeMs, loadedVoltage, unloadedVoltage, current, mAh);
}

void
DoubleReporter::reportEnd(int mAh) {
  reporter1.reportEnd(mAh);
  reporter2.reportEnd(mAh);
}

void
DoubleReporter::reportWaiting() {
  reporter1.reportWaiting();
  reporter2.reportWaiting();
}
