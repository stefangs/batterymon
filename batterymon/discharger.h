
#include "slot.h"

class Discharger {
 public:
   Discharger(Slot& slot)
   void loop();

 private:
   void idle();
   void discharge();

   Slot& slot
   enum State {idle, discharging} state;
   long startTime;
}