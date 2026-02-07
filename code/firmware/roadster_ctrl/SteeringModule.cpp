#include "SteeringModule.h"

SteeringModule::SteeringModule(int pin) {
  _pin = pin;
}

void SteeringModule::begin() {
  _servo.setPeriodHertz(SERVO_FREQ);
  _servo.attach(_pin, SERVO_USEC_MIN, SERVO_USEC_MAX);
  // Initialize to center
  _servo.writeMicroseconds(SERVO_CENTER);
}

void SteeringModule::setSteering(int usec) {
  // Constrain value to safe limits
  int safeVal = constrain(usec, SERVO_USEC_MIN, SERVO_USEC_MAX);
  _servo.writeMicroseconds(safeVal);
}
