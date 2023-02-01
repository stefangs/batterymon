
class Slot;
class Reporter;

class Discharger {
 public:
   Discharger(Slot& slot, Reporter& reporter);
   void loop();

 private:
   void doIdle();
   void doDischarge();
   void doEnded();

   Slot& slot;
   Reporter& reporter;
   enum State {idle, discharging, ended} state;
   long startTime;
   long nextSampleTime;
   long mA_Minutes;
};
