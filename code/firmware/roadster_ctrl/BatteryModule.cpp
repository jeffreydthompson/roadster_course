#include "BatteryModule.h"

BatteryModule::BatteryModule(int sda, int scl, int powerStatusPin) {
  _sda = sda;
  _scl = scl;
  _powerStatusPin = powerStatusPin;
  _onUsbPower = false;
  _voltage = 0.0f;
  _percent = 0.0f;
  _lastReadTime = 0;
  _newDataAvailable = false;
}

void BatteryModule::begin() {
  Wire.begin(_sda, _scl);

  if (_powerStatusPin >= 0) {
    // Open-drain output with an external pull-up (R1) to 3.3V
    pinMode(_powerStatusPin, INPUT);
    _onUsbPower = digitalRead(_powerStatusPin) == HIGH;
  }

  if (!_lipo.begin(&Wire)) {
    Serial.println("Error: MAX17048 not found!");
  } else {
    Serial.println("MAX17048 Fuel Gauge Found");
  }
}

void BatteryModule::update() {
  unsigned long now = millis();

  // Report plugging in / unplugging right away, not on the next read interval
  if (_powerStatusPin >= 0) {
    bool onUsb = digitalRead(_powerStatusPin) == HIGH;
    if (onUsb != _onUsbPower) {
      _onUsbPower = onUsb;
      _newDataAvailable = true;
      Serial.println(onUsb ? "Power: USB (charging)" : "Power: battery");
    }
  }

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

bool BatteryModule::isOnUsbPower() {
  return _onUsbPower;
}

bool BatteryModule::hasNewData() {
  if (_newDataAvailable) {
    _newDataAvailable = false;
    return true;
  }
  return false;
}
