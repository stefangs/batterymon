
#include <Arduino.h>
#include "commandhandler.h"
#define END_OF_STRING (0)
#define REPORT_COMMAND 'R'

CommandHandler::CommandHandler(ExtEEPromReporter* reporter) : reporter(reporter){
} 

void CommandHandler::loop() {
    if (Serial.available() > 0) {
        int incomingByte = Serial.read();
        if (incomingByte == '\n') {
            buffer[currentPosition] = END_OF_STRING;
            handleCommand(&buffer[0])
            currentPosition = 0;
        } else if (incomingByte != -1 && currentPosition < (BUFFER_SIZE - 1)) {
            buffer[currentPosition++] = incomingByte;
        }
}

void CommandHandler::handleCommand(char* commandString) {
    if (commandString[0] == REPORT_COMMAND) {
        reporter->printReport();
    }
}    
