
class Slot {
  public:
    Slot(char relayPin, char voltagePin);
    int voltage();
    void addLoad();
    void removeLoad();

  private:
    char relayPin;
    char voltagePin;
};
