
class Slot;
class Reporter;

class Discharger {
 public:
   Discharger(Slot& slot, Reporter& reporter);
   void loop();

 private:
   void doInitial();
   void doIdle();
   void doDischarge();
   void doEnded();
   bool isVoltageDroppingRapidly(int loadVoltage);

   Slot& slot;
   Reporter& reporter;
   enum State {initial, idle, discharging, ended} state;
   long startTime;
   long nextSampleTime;
   long mA_Minutes;
   int lastVoltage;
   char numberOfLargeDrops;
};
