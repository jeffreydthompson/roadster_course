#include "LightsModule.h"

LightsModule::LightsModule(int frontPin, int rearPin) {
  _frontPin = frontPin;
  _rearPin = rearPin;
  _headlights = false;
  _lastThrottle = 0;
  _reversing = false;
  _brakeUntil = 0;
  _rearLevel = -1;
}

void LightsModule::begin() {
  pinMode(_frontPin, OUTPUT);
  pinMode(_rearPin, OUTPUT);
  analogWrite(_frontPin, 0);
  analogWrite(_rearPin, 0);
}

void LightsModule::setHeadlights(bool on) {
  _headlights = on;
  analogWrite(_frontPin, on ? FULL_LEVEL : 0);
  applyRear();
}

void LightsModule::setThrottle(int throttle) {
  // Brake light on a real release: back to (near) zero after moving, or a
  // sharp drop. Small wobbles from a finger on the slider are ignored.
  int now = abs(throttle);
  int before = abs(_lastThrottle);
  bool released = now <= STOP_THRESHOLD && before > STOP_THRESHOLD;
  bool sharpDrop = before - now >= BRAKE_DROP;
  if (released || sharpDrop) {
    _brakeUntil = millis() + BRAKE_HOLD_MS;
  }
  _reversing = throttle < 0;
  _lastThrottle = throttle;
  applyRear();
}

void LightsModule::update() {
  applyRear();
}

void LightsModule::applyRear() {
  int level = 0;
  if (_reversing || (long)(_brakeUntil - millis()) > 0) {
    level = FULL_LEVEL;        // brake / reversing
  } else if (_headlights) {
    level = TAIL_LEVEL;        // tail light
  }

  // Only touch the PWM when the level actually changes
  if (level != _rearLevel) {
    analogWrite(_rearPin, level);
    _rearLevel = level;
  }
}
