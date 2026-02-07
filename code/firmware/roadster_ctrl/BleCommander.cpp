#include "BleCommander.h"

// Nordic UART Service UUIDs
#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

BleCommander::BleCommander(SteeringModule& steering, DriveModule& driveLeft, DriveModule& driveRight)
  : _steering(steering), _driveLeft(driveLeft), _driveRight(driveRight) {
  _deviceConnected = false;
  _newCommandAvailable = false;
  _lastCommand = 0;
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
  
  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06); 
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  
  Serial.println("BLE Commander Initialized. Waiting for client connection...");
}

void BleCommander::onConnect(BLEServer* pServer) {
  _deviceConnected = true;
  Serial.println("BLE Client Connected");
}

void BleCommander::onDisconnect(BLEServer* pServer) {
  _deviceConnected = false;
  Serial.println("BLE Client Disconnected");
  // Restart advertising so others can connect
  BLEDevice::startAdvertising();
}

void BleCommander::onWrite(BLECharacteristic* pCharacteristic) {
  String rxValue = pCharacteristic->getValue();

  if (rxValue.length() > 0) {
    // Only process the first character for now
    _lastCommand = rxValue[0];
    _newCommandAvailable = true;
  }
}

void BleCommander::update() {
  if (_newCommandAvailable) {
    processCommand(_lastCommand);
    _newCommandAvailable = false;
  }
}

void BleCommander::processCommand(char cmd) {
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
