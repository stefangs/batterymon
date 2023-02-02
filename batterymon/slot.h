
class Slot {
  public:
    Slot(char relayPin, char voltagePin, char greenLEDPin, char redLEDPin);
    int voltage();
    void addLoad();
    void removeLoad();
    void setGreenLED(boolean isOn);
    void setRedLED(boolean isOn);

  private:
    char relayPin;
    char voltagePin;
    char greenLEDPin;
    char redLEDPin;
};
