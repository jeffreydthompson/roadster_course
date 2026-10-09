#include "BatteryModule.h"

BatteryModule::BatteryModule(int sda, int scl) {
  _sda = sda;
  _scl = scl;
  _voltage = 0.0f;
  _percent = 0.0f;
  _lastReadTime = 0;
  _newDataAvailable = false;
}

void BatteryModule::begin() {
  Wire.begin(_sda, _scl);

  if (!_lipo.begin(&Wire)) {
    Serial.println("Error: MAX17048 not found!");
  } else {
    Serial.println("MAX17048 Fuel Gauge Found");
  }
}

void BatteryModule::update() {
  unsigned long now = millis();

  if (now - _lastReadTime >= READ_INTERVAL) {
    _lastReadTime = now;
    _voltage = _lipo.cellVoltage();
    _percent = _lipo.cellPercent();
    _newDataAvailable = true;
  }
}

float BatteryModule::getVoltage() {
  return _voltage;
}

float BatteryModule::getPercent() {
  return _percent;
}

bool BatteryModule::hasNewData() {
  if (_newDataAvailable) {
    _newDataAvailable = false;
    return true;
  }
  return false;
}
