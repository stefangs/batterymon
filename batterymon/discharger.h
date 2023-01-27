
#include "slot.h"

class Discharger {
 public:
   Discharger(Slot& slot);
   void loop();

 private:
   void doIdle();
   void doDischarge();
   void doEnded();

   Slot& slot;
   enum State {idle, discharging, ended} state;
   long startTime;
   long nextSampleTime;
};
