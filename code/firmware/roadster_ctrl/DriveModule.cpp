#include "DriveModule.h"

DriveModule::DriveModule(int pinA, int pinB) {
  _pinA = pinA;
  _pinB = pinB;
  _inverted = false;
}

void DriveModule::begin() {
  pinMode(_pinA, OUTPUT);
  pinMode(_pinB, OUTPUT);
  setSpeed(0);
}

void DriveModule::setInverted(bool inverted) {
  _inverted = inverted;
}

void DriveModule::setSpeed(int speed) {
  // Apply inversion if enabled
  if (_inverted) {
    speed = -speed;
  }

  // Constrain speed to valid PWM range
  int safeSpeed = constrain(speed, -255, 255);

  if (safeSpeed > 0) {
    // Forward
    analogWrite(_pinA, safeSpeed);
    analogWrite(_pinB, 0);
  } else if (safeSpeed < 0) {
    // Backward
    analogWrite(_pinA, 0);
    analogWrite(_pinB, -safeSpeed); // Convert to positive for PWM duty cycle
  } else {
    // Stop / Brake
    analogWrite(_pinA, 0);
    analogWrite(_pinB, 0);
  }
}
