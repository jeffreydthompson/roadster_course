#include <ESP32Servo.h>

const int SERVO_USEC_MAX = 2100;
const int SERVO_CENTER = 1520;
const int SERVO_USEC_MIN = 900;
int channel = 0;
int frequency = 333;
int resolution = 16;

const int motorA = 32;
const int motorB = 33;

const int servoPin = 25;

int servoPos = 1000;

int val = 0;

Servo servo;

void setup() {
  pinMode(motorA, OUTPUT);
  pinMode(motorB, OUTPUT);
  servo.setPeriodHertz(frequency); // hz servo
  servo.attach(servoPin, SERVO_USEC_MIN, SERVO_USEC_MAX);
}

void loop() {
  val = !val;

  servo.writeMicroseconds(SERVO_USEC_MIN);
  digitalWrite(motorA, 0);
  digitalWrite(motorB, 0);
  delay(2000);

  digitalWrite(motorA, val);
  digitalWrite(motorB, !val);
  servo.writeMicroseconds(SERVO_USEC_MAX);
  delay(2000);
}