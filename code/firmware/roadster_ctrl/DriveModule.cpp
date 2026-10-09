#include "DriveModule.h"

DriveModule::DriveModule(int pinA, int pinB, int pinSleep, int pinFault) {
  _pinA = pinA;
  _pinB = pinB;
  _pinSleep = pinSleep;
  _pinFault = pinFault;
  _inverted = false;
}

void DriveModule::begin() {
  pinMode(_pinA, OUTPUT);
  pinMode(_pinB, OUTPUT);
  setSpeed(0);

  if (_pinSleep >= 0) {
    pinMode(_pinSleep, OUTPUT);
    digitalWrite(_pinSleep, LOW);
  }

  if (_pinFault >= 0) {
    pinMode(_pinFault, INPUT);
  }

  if (_pinSleep >= 0) {
    digitalWrite(_pinSleep, HIGH);
    delay(2);
  }
}

void DriveModule::setInverted(bool inverted) {
  _inverted = inverted;
}

void DriveModule::setSleep(bool enabled) {
  if (_pinSleep < 0) {
    return;
  }

  digitalWrite(_pinSleep, enabled ? HIGH : LOW);
  if (enabled) {
    delay(2);
  }
}

bool DriveModule::isFaulted() const {
  if (_pinFault < 0) {
    return false;
  }

  return digitalRead(_pinFault) == LOW;
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
