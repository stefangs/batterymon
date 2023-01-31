
class EEPromReporter {
  public:
    void reportStart(int startVoltage, int loadVoltage, int current);
    void reportSample(long unsigned int timeMs, int loadedVoltage, int unloadedVoltage, int current, int mAh);
    void reportEnd();
    void reportWaiting();
  private:
    int slot;
    long nextSample;
};
