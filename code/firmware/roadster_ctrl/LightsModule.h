#ifndef LIGHTS_MODULE_H
#define LIGHTS_MODULE_H

#include <Arduino.h>

// Front (headlight) and rear (tail / brake) lights.
// Both are driven through constant-current transistor stages, so a
// HIGH pin turns the light on and PWM dims it.
class LightsModule {
  public:
    LightsModule(int frontPin, int rearPin);
    void begin();

    // Headlights on/off. With headlights on, the rear light glows as a tail light.
    void setHeadlights(bool on);

    // Feed the current throttle (-255..255) so the rear light can act as a brake light
    void setThrottle(int throttle);

    // Call every loop() to time out the brake light
    void update();

    // PWM levels (0-255)
    static const int TAIL_LEVEL = 40;
    static const int FULL_LEVEL = 255;

    // How long the brake light stays on after the throttle is released (ms)
    static const unsigned long BRAKE_HOLD_MS = 800;

    // Throttle at or below this counts as stopped
    static const int STOP_THRESHOLD = 5;

    // A drop this big in one update counts as braking
    static const int BRAKE_DROP = 30;

  private:
    int _frontPin;
    int _rearPin;
    bool _headlights;
    int _lastThrottle;
    bool _reversing;
    unsigned long _brakeUntil;
    int _rearLevel;

    void applyRear();
};

#endif
