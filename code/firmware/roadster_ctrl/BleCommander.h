#ifndef BLE_COMMANDER_H
#define BLE_COMMANDER_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "SteeringModule.h"
#include "DriveModule.h"

class BleCommander : public BLEServerCallbacks, public BLECharacteristicCallbacks {
  public:
    BleCommander(SteeringModule& steering, DriveModule& driveLeft, DriveModule& driveRight);
    void begin(const char* deviceName);
    void update();

    // BLE Server Callbacks
    void onConnect(BLEServer* pServer) override;
    void onDisconnect(BLEServer* pServer) override;

    // BLE Characteristic Callbacks (RX)
    void onWrite(BLECharacteristic* pCharacteristic) override;

  private:
    SteeringModule& _steering;
    DriveModule& _driveLeft;
    DriveModule& _driveRight;
    
    BLEServer* _pServer;
    BLECharacteristic* _pTxCharacteristic;
    bool _deviceConnected;
    char _lastCommand;
    bool _newCommandAvailable;

    void processCommand(char cmd);
};

#endif
