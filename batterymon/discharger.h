
class Slot;
class SerialReporter;

class Discharger {
 public:
   Discharger(Slot& slot, SerialReporter& reporter);
   void loop();

 private:
   void doIdle();
   void doDischarge();
   void doEnded();

   Slot& slot;
   SerialReporter& reporter;
   enum State {idle, discharging, ended} state;
   long startTime;
   long nextSampleTime;
   long mA_Minutes;
};
