//
//  BLEController.swift
//  RoadsterRemote
//
//  Created by Jeffrey Thompson on 2/7/26.
//

import CoreBluetooth
import Combine

fileprivate let SERVICE_UUID = CBUUID.init(string: "6E400001-B5A3-F393-E0A9-E50E24DCCA9E")
fileprivate let RX_UUID = CBUUID.init(string: "6E400002-B5A3-F393-E0A9-E50E24DCCA9E")
fileprivate let TX_UUID = CBUUID.init(string: "6E400003-B5A3-F393-E0A9-E50E24DCCA9E")

fileprivate let BATTERY_SERVICE_UUID = CBUUID.init(string: "180F")
fileprivate let BATTERY_LEVEL_CHARACTERISTIC_UUID = CBUUID.init(string: "2A19")

struct BLECommand {
    let throttle: Int16
    let steering: Int16
    let headlight: UInt8
    
    var payload: Data {
        var data = Data()
        
        let t = throttle.littleEndian
        let s = steering.littleEndian
        let h = headlight.littleEndian
        
        withUnsafeBytes(of: s) { data.append(contentsOf: $0) }
        withUnsafeBytes(of: t) { data.append(contentsOf: $0) }
        withUnsafeBytes(of: h) { data.append(contentsOf: $0) }
        
        return data
    }
}

class BLEController: NSObject {
    
    var centralManager: CBCentralManager!
    var peripheral: CBPeripheral?
    
    private var _connected = false {
        didSet {
            connected.send(self._connected)
        }
    }
    var connected = PassthroughSubject<Bool, Never>()
    var batteryLevel = PassthroughSubject<UInt8, Never>()
    
    var commandChar : CBCharacteristic?
    var subscribeChar: CBCharacteristic?
    
    override init() {
        super.init()
        centralManager = CBCentralManager(delegate: self, queue: .global(qos: .default))
    }
    
    func send(command: BLECommand) {
        guard
            _connected,
            let peripheral = peripheral,
            let commandChar = commandChar
        else { return }
        
        let payload = command.payload
        
        peripheral.writeValue(payload, for: commandChar, type: .withResponse)
    }
}

extension BLEController: CBCentralManagerDelegate {
    
    func centralManagerDidUpdateState(_ central: CBCentralManager) {
        switch central.state {
        case .poweredOn:
            centralManager.scanForPeripherals(withServices: [SERVICE_UUID])
        default:
            break
        }
    }
    
    func centralManager(_ central: CBCentralManager, didConnect peripheral: CBPeripheral) {
        _connected = true
        self.peripheral?.discoverServices([SERVICE_UUID, BATTERY_SERVICE_UUID])
    }
    
    func centralManager(_ central: CBCentralManager, didDisconnectPeripheral peripheral: CBPeripheral, error: (any Error)?) {
        _connected = false
    }
    
    func centralManager(_ central: CBCentralManager, didDiscover peripheral: CBPeripheral, advertisementData: [String : Any], rssi RSSI: NSNumber) {
        
        if peripheral.name == "Roadster_ESP32" {
            self.peripheral = peripheral
            self.centralManager.connect(self.peripheral!, options: nil)
            self.centralManager.stopScan()
            
            peripheral.delegate = self
        }
    }
    
    func centralManager(_ central: CBCentralManager, connectionEventDidOccur event: CBConnectionEvent, for peripheral: CBPeripheral) {
        switch event {
        case .peerDisconnected:
            self._connected = false
        case .peerConnected:
            self._connected = true
        @unknown default:
            break
        }
    }
}

extension BLEController: CBPeripheralDelegate {
    
    func peripheral(_ peripheral: CBPeripheral, didDiscoverCharacteristicsFor service: CBService, error: (any Error)?) {
        
        if service.uuid == SERVICE_UUID {
            for c in service.characteristics ?? [] {
                if c.uuid == RX_UUID {
                    self.commandChar = c
                }
                
                if c.uuid == TX_UUID {
                    self.subscribeChar = c
                }
            }
        }
        
        if service.uuid == BATTERY_SERVICE_UUID {
            for c in service.characteristics ?? [] {
                
                if c.uuid == BATTERY_LEVEL_CHARACTERISTIC_UUID {
                    self.peripheral?.setNotifyValue(true, for: c)
                }
            }
        }
    }
    
    func peripheral(
        _ peripheral: CBPeripheral,
        didDiscoverServices error: (any Error)?) {
            
            for s in peripheral.services ?? [] {
                if s.uuid == BATTERY_SERVICE_UUID {
                    peripheral.discoverCharacteristics([BATTERY_LEVEL_CHARACTERISTIC_UUID], for: s)
                }
                
                if s.uuid == SERVICE_UUID {
                    peripheral.discoverCharacteristics([RX_UUID, TX_UUID], for: s)
                }
            }
    }
    
    func peripheral(_ peripheral: CBPeripheral, didUpdateValueFor characteristic: CBCharacteristic, error: (any Error)?) {
        if characteristic.uuid == BATTERY_LEVEL_CHARACTERISTIC_UUID {
            
            let data = characteristic.value
            let battery = data?.first ?? 0
            batteryLevel.send(battery)
        }
    }
}
