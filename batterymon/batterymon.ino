

#include "slot.h"

#define MINUTE_IN_MS (60000)
#define LOAD_PAUSE (200)
#define INPUT_BUFFER_SIZE 20

static char inputBuffer[INPUT_BUFFER_SIZE];

Slot slot0(2, A0);

void setup() {
  Serial.begin(115200);
  slot0.addLoad();
  delay(1000);
}

char readInput() {
  while (Serial.available() == 0) ;
  char result = Serial.readBytesUntil('\n', inputBuffer, INPUT_BUFFER_SIZE);
  inputBuffer[result] = 0;
  return result;
}

void readAnalogValue(char* in) {
  char pin = in[1] - '0';
  if ((pin < 0) || (pin > 4)) {
    Serial.println("Bad pin");
    return;
  }  
  int value = analogRead (A0 + pin);
  Serial.println(value);
}

void writeDigitalValue(char* in) {
  char pin = in[1] - '0';
  if ((pin < 0) || (pin > 7)) {
    Serial.println("Bad pin");
    return;
  }
  digitalWrite(pin, (in[2] == '1') ? HIGH : LOW);
  Serial.println(in);
}

void loop() {
  int loaded = slot0.voltage();
  slot0.removeLoad();
  delay(LOAD_PAUSE);
  int unloaded = slot0.voltage();
  slot0.addLoad();
  Serial.print(millis() / MINUTE_IN_MS);
  Serial.print(',');  
  Serial.print(loaded);
  Serial.print(',');
  Serial.println(unloaded);
  delay(MINUTE_IN_MS - LOAD_PAUSE);
}


void loop2() {
  char size = readInput();
  if (inputBuffer[0] == 'A' && size >= 2) {
    readAnalogValue(&inputBuffer[0]);
  } else if (inputBuffer[0] == 'D' && size >= 3) {
    writeDigitalValue(&inputBuffer[0]);
  }
}
