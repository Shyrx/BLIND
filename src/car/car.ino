#include <Servo.h>

Servo servo; // create servo object to control a servo

int PIN = 13;

int front = 92;
int dir = 7;
int spd = 6;
int forward = LOW;
int backward = HIGH;
int minSpd = 0;
int maxSpd = 255;

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

void resetDir()
{
    servo.write(front);
}

void handleInput(char input)
{
    Serial.write(input);
}

void setup()
{
    servo.attach(PIN);
    pinMode(PIN, OUTPUT);
    pinMode(dir, OUTPUT);
    pinMode(spd, OUTPUT);
    resetDir();

    Serial.begin(115200);
    while (!Serial)
    {
        ;
    }
    delay(300);
    moveForward(170);
}

int receivedByte = 0;
bool negative = false;
int i = 0;
char angle[3] = { 0 };

void resetString(char *str, int nb)
{
    for (int j = 0; j < nb; j++)
        str[j] = 0;
}

void loop()
{
    if (Serial.available() > 0)
    {
        receivedByte = Serial.read();
        Serial.write(':');
        Serial.write(receivedByte);
        Serial.write('\n');
        if (receivedByte == '-')
        {
            negative = true;
        }
        else if (receivedByte == '\n')
        {
            int sign = negative ? -1 : 1;
            turn(front + sign * atoi(angle));
            resetString(angle, 3);
            i = 0;
            negative = false;
        }
        else
        {
            angle[i++] = receivedByte;
        }
    }

    delay(100);
}
