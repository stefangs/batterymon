
#pragma once

#define BUFFER_SIZE (5)

class CommandHandler {
  public:
    CommandHandler(ExtEEPromReporter* reporter);
    void loop();

  private:
  void handleCommand(char* commandString);
    char buffer[BUFFER_SIZE];
    int currentPosition;
    ExtEEPromReporter* reporter;
};