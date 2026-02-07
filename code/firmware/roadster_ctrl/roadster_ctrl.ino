#include "SteeringModule.h"
#include "DriveModule.h"
#include "SerialCommander.h"

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
SerialCommander commander(steering, driveLeft, driveRight);

void setup() {
  // Initialize Modules
  steering.begin();
  driveLeft.begin();
  driveRight.begin();
  
  // Initialize Commander
  commander.begin(115200);
}

void loop() {
  commander.update();
}
