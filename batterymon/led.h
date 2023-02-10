
#pragma once

class Led {
  public:
    Led(char pin);
    void on();
    void off();
    void blinkOn(int periodMs, char dutyPercentage);
    void blinkOff();
    void loop();

  private:
    char pin;
    int periodMs;
    char dutyPercentage;
};
