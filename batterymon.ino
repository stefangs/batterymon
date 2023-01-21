

#define INPUT_BUFFER_SIZE 20

static char inputBuffer[INPUT_BUFFER_SIZE];

void setup() {
  pinMode(A0, INPUT);
  digitalWrite(0, LOW);
  pinMode(0, OUTPUT);
  digitalWrite(1, LOW);
  pinMode(1, OUTPUT);
  digitalWrite(2, LOW);
  pinMode(2, OUTPUT);
  Serial.begin(115200);
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
  char size = readInput();
  if (inputBuffer[0] == 'A' && size >= 2) {
    readAnalogValue(&inputBuffer[0]);
  } else if (inputBuffer[0] == 'D' && size >= 3) {
    writeDigitalValue(&inputBuffer[0]);
  }
}
