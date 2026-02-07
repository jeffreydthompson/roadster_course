#ifndef DRIVE_MODULE_H
#define DRIVE_MODULE_H

#include <Arduino.h>

class DriveModule {
  public:
    DriveModule(int pinA, int pinB);
    void begin();
    
    // speed: -255 (full reverse) to 255 (full forward). 0 is stop.
    void setSpeed(int speed);

  private:
    int _pinA;
    int _pinB;
};

#endif
