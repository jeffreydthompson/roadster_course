#ifndef DRIVE_MODULE_H
#define DRIVE_MODULE_H

#include <Arduino.h>

class DriveModule {
  public:
    DriveModule(int pinA, int pinB, int pinSleep = -1, int pinFault = -1);
    void begin();
    
    // speed: -255 (full reverse) to 255 (full forward). 0 is stop.
    void setSpeed(int speed);
    
    // Invert motor direction (useful if wired backwards)
    void setInverted(bool inverted);

    void setSleep(bool enabled);
    bool isFaulted() const;

  private:
    int _pinA;
    int _pinB;
    int _pinSleep;
    int _pinFault;
    bool _inverted;
};

#endif
