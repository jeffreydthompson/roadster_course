#ifndef BATTERY_MODULE_H
#define BATTERY_MODULE_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MAX1704X.h>

class BatteryModule {
  public:
    BatteryModule(int sda, int scl);
    void begin();
    void update();
    float getVoltage();
    float getPercent();

    bool hasNewData();

  private:
    Adafruit_MAX17048 _lipo;
    int _sda;
    int _scl;
    float _voltage;
    float _percent;
    unsigned long _lastReadTime;
    bool _newDataAvailable;

    static const unsigned long READ_INTERVAL = 2000;
};

#endif
