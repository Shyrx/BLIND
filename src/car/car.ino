#include <Servo.h>

int PIN = 13;

int front = 92;
int dir = 7;
int spd = 6;
int forward = LOW;
int backward = HIGH;
int minSpd = 0;
int maxSpd = 255;

int receivedByte;

void moveForward(int rotation)
{
    digitalWrite(dir, forward);
    analogWrite(spd, rotation);
}

void stop()
{
    analogWrite(spd, minSpd);
}

void turn(int degree)
{
    if (degree < 72)
        degree = 72;
    if (degree > 108)
        degree = 108;

    servo.write(degree);
}

void handleInput(char input) {
    Serial.write(input);
}

void setup() {
  servo.attach(PIN);
  pinMode(PIN, OUTPUT);
  pinMode(dir, OUTPUT);
  pinMode(spd, OUTPUT);
  resetDir();

  Serial.begin(115200);
  while (!Serial) {
    ;
  }
  delay(300);
}

void loop() {
  if (Serial.available() > 0) {
    receivedByte = Serial.read();
    handleInput(receivedByte);
  }

  delay(50);
}
