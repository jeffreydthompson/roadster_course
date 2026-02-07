#include "SteeringModule.h"
#include "DriveModule.h"
#include "SerialCommander.h"
#include "BleCommander.h"

// Hardware Configuration
const int PIN_SERVO = 10;

// Left Motor (L9110s)
const int PIN_MOTOR_LEFT_A = 4;
const int PIN_MOTOR_LEFT_B = 5;

// Right Motor (L9110s)
const int PIN_MOTOR_RIGHT_A = 6;
const int PIN_MOTOR_RIGHT_B = 7;

// Global Objects
SteeringModule steering(PIN_SERVO);
DriveModule driveLeft(PIN_MOTOR_LEFT_A, PIN_MOTOR_LEFT_B);
DriveModule driveRight(PIN_MOTOR_RIGHT_A, PIN_MOTOR_RIGHT_B);

// Control Interfaces
SerialCommander serialCmd(steering, driveLeft, driveRight);
BleCommander bleCmd(steering, driveLeft, driveRight);

void setup() {
  // Initialize Modules
  steering.begin();
  driveLeft.begin();
  driveRight.begin();
  
  // Correct for wiring: Left motor is physically reversed relative to Right
  driveLeft.setInverted(true);
  
  // Initialize Serial Commander
  serialCmd.begin(115200);

  // Initialize BLE Commander
  bleCmd.begin("Roadster_ESP32");
}

void loop() {
  serialCmd.update();
  bleCmd.update();
}
