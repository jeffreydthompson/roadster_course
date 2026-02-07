#ifndef SERIAL_COMMANDER_H
#define SERIAL_COMMANDER_H

#include <Arduino.h>
#include "SteeringModule.h"
#include "DriveModule.h"

class SerialCommander {
  public:
    SerialCommander(SteeringModule& steering, DriveModule& driveLeft, DriveModule& driveRight);
    void begin(long baudRate);
    void update();

  private:
    SteeringModule& _steering;
    DriveModule& _driveLeft;
    DriveModule& _driveRight;
    
    void processCommand(char cmd);
};

#endif
