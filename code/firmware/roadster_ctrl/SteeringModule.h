#ifndef STEERING_MODULE_H
#define STEERING_MODULE_H

#include <Arduino.h>
#include <ESP32Servo.h>

class SteeringModule {
  public:
    SteeringModule(int pin);
    void begin();
    void setSteering(int usec);
    
    // Constants from original servo.ino
    static const int SERVO_USEC_MIN = 900;
    static const int SERVO_USEC_MAX = 2100;
    static const int SERVO_CENTER = 1520;
    static const int SERVO_FREQ = 333;

  private:
    int _pin;
    Servo _servo;
};

#endif
