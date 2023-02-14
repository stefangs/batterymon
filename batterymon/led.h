
#pragma once

class Led {
  public:
    Led(char pin);
    void on();
    void off();
    void blinkOn(int periodMs, char dutyPercentage);
    void blinkOff();
    void blinkCount(char count);
    void loop();

  private:
    inline bool isBlinkCountOn();
    char pin;
    int periodMs;
    char dutyPercentage;
    char count;
};
