int receivedByte;

void handleInput(char input) {
    Serial.write(input);
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  Serial.begin(115200);
  while (!Serial) {
    ;
  }

  // turn on LED when ready to communicate
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
  if (Serial.available() > 0) {
    receivedByte = Serial.read();
    handleInput(receivedByte);
  }

  delay(50);
}
