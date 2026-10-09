#include "BleCommander.h"
#include <cstring>

// Nordic UART Service UUIDs
#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

// Battery Service UUIDs (Standard Bluetooth SIG)
#define BATTERY_SERVICE_UUID "180F"
#define BATTERY_LEVEL_UUID   "2A19"

// Custom: power source, 1 byte (1 = USB power / charging, 0 = battery)
#define POWER_SOURCE_UUID    "8FDDF80E-8D10-4B50-9466-FCE56AF3B124"

BleCommander::BleCommander(SteeringModule& steering, DriveModule& driveLeft, DriveModule& driveRight, BatteryModule& battery, LightsModule& lights)
  : _steering(steering), _driveLeft(driveLeft), _driveRight(driveRight), _battery(battery), _lights(lights) {
  _deviceConnected = false;
  _disconnectPending = false;
  _newLegacyCommand = false;
  _lastLegacyCommand = 0;
  _newPacketAvailable = false;
  _frontHeadlightOn = false;
  _lastThrottle = 0;
}

void BleCommander::begin(const char* deviceName) {
  BLEDevice::init(deviceName);
  _pServer = BLEDevice::createServer();
  _pServer->setCallbacks(this);

  BLEService* pService = _pServer->createService(SERVICE_UUID);

  // Create TX Characteristic (Notify)
  _pTxCharacteristic = pService->createCharacteristic(
                         CHARACTERISTIC_UUID_TX,
                         BLECharacteristic::PROPERTY_NOTIFY
                       );
  _pTxCharacteristic->addDescriptor(new BLE2902());

  // Create RX Characteristic (Write)
  BLECharacteristic* pRxCharacteristic = pService->createCharacteristic(
                                           CHARACTERISTIC_UUID_RX,
                                           BLECharacteristic::PROPERTY_WRITE
                                         );
  pRxCharacteristic->setCallbacks(this);

  pService->start();

  // Battery Service
  BLEService* pBatService = _pServer->createService(BATTERY_SERVICE_UUID);
  _pBatteryCharacteristic = pBatService->createCharacteristic(
                              BATTERY_LEVEL_UUID,
                              BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
                            );
  _pBatteryCharacteristic->addDescriptor(new BLE2902());
  _pPowerSourceCharacteristic = pBatService->createCharacteristic(
                                  POWER_SOURCE_UUID,
                                  BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
                                );
  _pPowerSourceCharacteristic->addDescriptor(new BLE2902());
  pBatService->start();

  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->addServiceUUID(BATTERY_SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06); // helps with iPhone connection issues
  pAdvertising->setMaxPreferred(0x12);
  BLEDevice::startAdvertising();
  
  Serial.println("BLE Commander Initialized. Waiting for client connection...");
}

void BleCommander::onConnect(BLEServer* pServer) {
  _deviceConnected = true;
  Serial.println("BLE Client Connected");
}

void BleCommander::onDisconnect(BLEServer* pServer) {
  _deviceConnected = false;
  _disconnectPending = true; // failsafe: stop the motors in update()
  Serial.println("BLE Client Disconnected");
  // Restart advertising so others can connect
  BLEDevice::startAdvertising();
}

void BleCommander::onWrite(BLECharacteristic* pCharacteristic) {
  uint8_t* data = pCharacteristic->getData();
  size_t len = pCharacteristic->getLength();

  if (len == sizeof(ControlPacket)) {
    memcpy(&_lastPacket, data, sizeof(ControlPacket));
    _newPacketAvailable = true;
  } else if (len == 4) {
    memset(&_lastPacket, 0, sizeof(ControlPacket));
    memcpy(&_lastPacket, data, len);
    _lastPacket.headlight = 0;
    _newPacketAvailable = true;
  } else if (len == 1) {
    _lastLegacyCommand = (char)data[0];
    _newLegacyCommand = true;
  }
}

void BleCommander::update() {
  if (_disconnectPending) {
    // Lost the controller - drop anything still queued so the car
    // can't drive off on a stale packet, then stop.
    _newPacketAvailable = false;
    _newLegacyCommand = false;
    _disconnectPending = false;
    _driveLeft.setSpeed(0);
    _driveRight.setSpeed(0);
    _lights.setThrottle(0);
    Serial.println("Failsafe: motors stopped");
    return;
  }

  if (_newPacketAvailable) {
    processPacket(_lastPacket);
    _newPacketAvailable = false;
  } else if (_newLegacyCommand) {
    processLegacyCommand(_lastLegacyCommand);
    _newLegacyCommand = false;
  }

  if (_deviceConnected && _battery.hasNewData()) {
    updateBatteryLevel();
  }
}

void BleCommander::processPacket(ControlPacket packet) {
  _lastThrottle = packet.throttle;
  _frontHeadlightOn = packet.headlight > 0;
  _lights.setHeadlights(_frontHeadlightOn);
  _lights.setThrottle(packet.throttle);

  _driveLeft.setSpeed(packet.throttle);
  _driveRight.setSpeed(packet.throttle);

  int servoUsec = map(packet.steering, -100, 100,
                      SteeringModule::SERVO_USEC_MIN,
                      SteeringModule::SERVO_USEC_MAX);
  _steering.setSteering(servoUsec);
}

void BleCommander::processLegacyCommand(char cmd) {
  // Simple check to send feedback via Notify
  String feedback = "cmd: ";
  feedback += cmd;
  
  switch (cmd) {
    case 'f': // Forward Full Speed
      _driveLeft.setSpeed(255);
      _driveRight.setSpeed(255);
      break;
      
    case 'b': // Backward 25% Speed
      _driveLeft.setSpeed(-64); 
      _driveRight.setSpeed(-64);
      break;
      
    case 's': // Stop
      _driveLeft.setSpeed(0);
      _driveRight.setSpeed(0);
      break;
      
    case 'l': // Left
      _steering.setSteering(SteeringModule::SERVO_USEC_MIN);
      break;
      
    case 'r': // Right
      _steering.setSteering(SteeringModule::SERVO_USEC_MAX);
      break;
      
    case 'c': // Center
      _steering.setSteering(SteeringModule::SERVO_CENTER);
      break;
      
    default:
      feedback = "Unknown cmd: ";
      feedback += cmd;
      break;
  }

  // Echo back to Serial for debugging
  Serial.println(feedback);

  // Send feedback to BLE Client if connected
  if (_deviceConnected) {
    _pTxCharacteristic->setValue((uint8_t*)feedback.c_str(), feedback.length());
    _pTxCharacteristic->notify();
  }
}

void BleCommander::updateBatteryLevel() {
  float pct = _battery.getPercent();
  uint8_t level = (uint8_t)constrain((int)pct, 0, 100);
  _pBatteryCharacteristic->setValue(&level, 1);
  _pBatteryCharacteristic->notify();

  uint8_t onUsb = _battery.isOnUsbPower() ? 1 : 0;
  _pPowerSourceCharacteristic->setValue(&onUsb, 1);
  _pPowerSourceCharacteristic->notify();
}
