#include "SteeringModule.h"
#include "DriveModule.h"
#include "BatteryModule.h"
#include "LightsModule.h"
#include "SerialCommander.h"
#include "BleCommander.h"
#include <driver/gpio.h>
#include <esp_sleep.h>

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

// While shut down for low battery, wake this often to check for USB power
// or a recharged cell
const uint64_t LOW_BATTERY_CHECK_US = 60ULL * 1000000ULL;

// Driven low and held there through deep sleep: motors coasting with the
// DRV8833s asleep, servo signal off, lights off
const int SLEEP_LOW_PINS[] = {
  PIN_MOTOR_LEFT_A, PIN_MOTOR_LEFT_B, PIN_MOTOR_RIGHT_A, PIN_MOTOR_RIGHT_B,
  PIN_L_SLEEP, PIN_R_SLEEP, PIN_SERVO, PIN_WLED, PIN_RLED,
};

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
  if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER) {
    // Only the low-battery shutdown sets a timer wake
    checkBatteryAfterShutdown();
  }
  releaseSleepPins();

  // Initialize Modules
  // Steering must attach first: ESP32Servo claims LEDC channel 0 directly,
  // and analogWrite() would otherwise have handed that channel to the lights,
  // leaving the servo and headlights sharing one PWM signal.
  if (ENABLE_STEERING) {
    steering.begin();
  }

  lights.begin();

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
  BatteryState batteryState = battery.getState();
  lights.setLowBattery(batteryState == BatteryState::Low);
  lights.update();

  if (batteryState == BatteryState::Critical) {
    shutDownForLowBattery();
  }
}

// Stop, warn, and sleep before the cell is run flat. The car stays asleep
// until it's on USB power or the cell has been recharged, or the power
// switch is cycled.
void shutDownForLowBattery() {
  Serial.println("Battery critical: shutting down");
  driveLeft.setSpeed(0);
  driveRight.setSpeed(0);
  lights.flashWarning();
  sleepUntilBatteryCheck();
}

void sleepUntilBatteryCheck() {
  // GPIOs float in deep sleep unless held
  for (int pin : SLEEP_LOW_PINS) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
    gpio_hold_en((gpio_num_t)pin);
  }
  gpio_deep_sleep_hold_en();

  esp_sleep_enable_timer_wakeup(LOW_BATTERY_CHECK_US);
  esp_deep_sleep_start();
}

void releaseSleepPins() {
  for (int pin : SLEEP_LOW_PINS) {
    gpio_hold_dis((gpio_num_t)pin);
  }
  gpio_deep_sleep_hold_dis();
}

// Woken from the low-battery shutdown: go straight back to sleep unless the
// car is on USB power or the cell has recovered
void checkBatteryAfterShutdown() {
  battery.begin();
  delay(500); // let the fuel gauge take a reading after its reset

  bool onUsb = battery.isOnUsbPower();
  float volts = battery.readVoltageNow();
  if (!onUsb && volts < BatteryModule::RECOVER_VOLTAGE) {
    sleepUntilBatteryCheck();
  }
}
