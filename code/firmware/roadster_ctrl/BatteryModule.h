#ifndef BATTERY_MODULE_H
#define BATTERY_MODULE_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MAX1704X.h>

// Low-battery protection. Only applies when running on the battery.
enum class BatteryState {
  Ok,
  Low,       // warn and limit the throttle
  Critical,  // stop and shut down to save the cell
};

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

    BatteryState getState();

    // Reads the cell voltage straight away instead of waiting for update().
    // Used on wake from the low-battery shutdown.
    float readVoltageNow();

    // Cell voltage thresholds (V). They are checked under load, so they sit
    // a little below where the resting voltage would put them.
    static constexpr float LOW_VOLTAGE = 3.4f;
    static constexpr float CUTOFF_VOLTAGE = 3.2f;
    // After a shutdown, wake back up only once the resting cell is above this
    static constexpr float RECOVER_VOLTAGE = 3.6f;
    // Readings in a row needed to change state, so a burst of throttle
    // sagging the cell doesn't trip it (READ_INTERVAL apart)
    static const int CONFIRM_READS = 3;

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
    BatteryState _state;
    int _lowReads;
    int _criticalReads;

    void updateState();

    static const unsigned long READ_INTERVAL = 2000;
};

#endif
