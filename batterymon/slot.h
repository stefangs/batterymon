
#include "led.h"

class Slot {
  public:
    Slot(char relayPin, char voltagePin, char greenLEDPin, char redLEDPin);
    int voltage();
    void addLoad();
    void removeLoad();
    Led& getGreenLED();
    Led& getRedLED();
    void loop();

  private:
    char relayPin;
    char voltagePin;
    Led greenLED;
    Led redLED;
};
