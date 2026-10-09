#ifndef BLE_COMMANDER_H
#define BLE_COMMANDER_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "SteeringModule.h"
#include "DriveModule.h"
#include "BatteryModule.h"

// Binary Packet Structure (Matches App)
#pragma pack(1)
struct ControlPacket {
  int16_t steering; // -100 to 100
  int16_t throttle; // -255 to 255
  uint8_t headlight; // 0 = off, 1 = on
};
#pragma pack()

class BleCommander : public BLEServerCallbacks, public BLECharacteristicCallbacks {
  public:
    BleCommander(SteeringModule& steering, DriveModule& driveLeft, DriveModule& driveRight, BatteryModule& battery);
    void begin(const char* deviceName);
    void update();
    void setLedPin(int pin);

    // BLE Server Callbacks
    void onConnect(BLEServer* pServer) override;
    void onDisconnect(BLEServer* pServer) override;

    // BLE Characteristic Callbacks (RX)
    void onWrite(BLECharacteristic* pCharacteristic) override;

  private:
    SteeringModule& _steering;
    DriveModule& _driveLeft;
    DriveModule& _driveRight;
    BatteryModule& _battery;
    
    BLEServer* _pServer;
    BLECharacteristic* _pTxCharacteristic;
    BLECharacteristic* _pBatteryCharacteristic;
    // Written from the BLE task, read from loop()
    volatile bool _deviceConnected;
    volatile bool _disconnectPending;
    volatile char _lastLegacyCommand;
    volatile bool _newLegacyCommand;

    ControlPacket _lastPacket;
    volatile bool _newPacketAvailable;
    bool _frontHeadlightOn;
    int _lastThrottle;
    int _ledPin;
    bool _ledState;

    void processLegacyCommand(char cmd);
    void processPacket(ControlPacket packet);
    void updateBatteryLevel();
    void setLedState(bool on);
};

#endif
