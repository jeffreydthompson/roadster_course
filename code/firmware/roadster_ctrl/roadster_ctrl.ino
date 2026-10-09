#include "SteeringModule.h"
#include "DriveModule.h"
#include "BatteryModule.h"
#include "LightsModule.h"
#include "SerialCommander.h"
#include "BleCommander.h"

// Hardware Configuration
const int PIN_SERVO = 45;

// Left Motor (DRV8833)
const int PIN_MOTOR_LEFT_A = 6;
const int PIN_MOTOR_LEFT_B = 5;

// Right Motor (DRV8833)
const int PIN_MOTOR_RIGHT_A = 39;
const int PIN_MOTOR_RIGHT_B = 40;

// DRV8833 Control
const int PIN_L_SLEEP = 11;
const int PIN_R_SLEEP = 38;
const int PIN_L_FAULT = 7;
const int PIN_R_FAULT = 34;

// Battery I2C
const int PIN_I2C_SDA = 8;
const int PIN_I2C_SCL = 9;

// Power status: TPS2116 ST output (HIGH = USB power, LOW = battery)
const int PIN_POWER_ST = 26;

// Lights (F.LED and B.LED connectors)
const int PIN_WLED = 42; // front / headlights
const int PIN_RLED = 41; // rear / tail + brake

const bool ENABLE_SERIAL = false;
const bool ENABLE_STEERING = true;
const bool ENABLE_DRIVE = true;

// Global Objects
SteeringModule steering(PIN_SERVO);
DriveModule driveLeft(PIN_MOTOR_LEFT_A, PIN_MOTOR_LEFT_B, PIN_L_SLEEP, PIN_L_FAULT);
DriveModule driveRight(PIN_MOTOR_RIGHT_A, PIN_MOTOR_RIGHT_B, PIN_R_SLEEP, PIN_R_FAULT);
BatteryModule battery(PIN_I2C_SDA, PIN_I2C_SCL, PIN_POWER_ST);
LightsModule lights(PIN_WLED, PIN_RLED);

// Control Interfaces
SerialCommander serialCmd(steering, driveLeft, driveRight);
BleCommander bleCmd(steering, driveLeft, driveRight, battery, lights);

void setup() {
  // Initialize Modules
  lights.begin();

  if (ENABLE_STEERING) {
    steering.begin();
  }

  if (ENABLE_DRIVE) {
    driveLeft.begin();
    driveRight.begin();
  }

  battery.begin();
  
  // Correct for wiring: Left motor is physically reversed relative to Right
  if (ENABLE_DRIVE) {
    driveLeft.setInverted(true);
    driveRight.setInverted(true);
  }
  
  // Initialize Serial Commander
  if (ENABLE_SERIAL) {
    serialCmd.begin(115200);
  }

  // Initialize BLE Commander
  bleCmd.begin("Open Roadster");
}

void loop() {
  if (ENABLE_SERIAL) {
    serialCmd.update();
  }
  bleCmd.update();

  battery.update();
  lights.update();
}
