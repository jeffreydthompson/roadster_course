#ifndef BATTERY_MODULE_H
#define BATTERY_MODULE_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MAX1704X.h>

class BatteryModule {
  public:
    // powerStatusPin: TPS2116 ST output (HIGH = running on USB, LOW = on battery)
    BatteryModule(int sda, int scl, int powerStatusPin = -1);
    void begin();
    void update();
    float getVoltage();
    float getPercent();

    // True when the car is running on USB power (and the battery is charging)
    bool isOnUsbPower();

    bool hasNewData();

  private:
    Adafruit_MAX17048 _lipo;
    int _sda;
    int _scl;
    int _powerStatusPin;
    bool _onUsbPower;
    float _voltage;
    float _percent;
    unsigned long _lastReadTime;
    bool _newDataAvailable;

    static const unsigned long READ_INTERVAL = 2000;
};

#endif
