#include "SerialCommander.h"

SerialCommander::SerialCommander(SteeringModule& steering, DriveModule& driveLeft, DriveModule& driveRight)
  : _steering(steering), _driveLeft(driveLeft), _driveRight(driveRight) {
}

void SerialCommander::begin(long baudRate) {
  Serial.begin(baudRate);
  while (!Serial) {
    ; // Wait for serial port to connect
  }
  Serial.println("Roadster Controller Ready.");
  Serial.println("Commands: f=Forward, b=Back (25%), s=Stop, l=Left, r=Right, c=Center");
}

void SerialCommander::update() {
  if (Serial.available() > 0) {
    char cmd = (char)Serial.read();
    processCommand(cmd);
  }
}

void SerialCommander::processCommand(char cmd) {
  switch (cmd) {
    case 'f': // Forward Full Speed
      _driveLeft.setSpeed(255);
      _driveRight.setSpeed(255);
      Serial.println("cmd: Forward");
      break;
      
    case 'b': // Backward 25% Speed
      _driveLeft.setSpeed(-64); // approx 25% of 255
      _driveRight.setSpeed(-64);
      Serial.println("cmd: Backward (25%)");
      break;
      
    case 's': // Stop
      _driveLeft.setSpeed(0);
      _driveRight.setSpeed(0);
      Serial.println("cmd: Stop");
      break;
      
    case 'l': // Left
      _steering.setSteering(SteeringModule::SERVO_USEC_MIN);
      Serial.println("cmd: Left");
      break;
      
    case 'r': // Right
      _steering.setSteering(SteeringModule::SERVO_USEC_MAX);
      Serial.println("cmd: Right");
      break;
      
    case 'c': // Center
      _steering.setSteering(SteeringModule::SERVO_CENTER);
      Serial.println("cmd: Center");
      break;
      
    default:
      // Ignore newlines or unknown commands
      if (cmd != '\n' && cmd != '\r') {
        Serial.print("Unknown command: ");
        Serial.println(cmd);
      }
      break;
  }
}
