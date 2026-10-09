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

// A car that drops out of scan results for this long is removed from the list
fileprivate let STALE_CAR_TIMEOUT: TimeInterval = 5

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

/// A Roadster found while scanning
struct DiscoveredCar: Identifiable, Equatable {
    let id: UUID
    var name: String
    var rssi: Int
    var lastSeen: Date

    /// Short tag to tell identical-looking cars apart (iOS assigns the UUID per phone)
    var shortID: String {
        String(id.uuidString.suffix(4))
    }

    /// 0...1 for signal strength bars
    var signalStrength: Double {
        switch rssi {
        case (-60)...: return 1.0
        case (-70)...: return 0.75
        case (-80)...: return 0.5
        default: return 0.25
        }
    }
}

enum ConnectionState: Equatable {
    case disconnected
    case connecting(DiscoveredCar)
    case connected(DiscoveredCar)

    var car: DiscoveredCar? {
        switch self {
        case .disconnected: return nil
        case .connecting(let car), .connected(let car): return car
        }
    }
}

class BLEController: NSObject {

    var centralManager: CBCentralManager!
    var peripheral: CBPeripheral?

    var connectionState = CurrentValueSubject<ConnectionState, Never>(.disconnected)
    var cars = CurrentValueSubject<[DiscoveredCar], Never>([])
    var bluetoothState = CurrentValueSubject<CBManagerState, Never>(.unknown)
    var batteryLevel = PassthroughSubject<UInt8, Never>()

    var commandChar : CBCharacteristic?
    var subscribeChar: CBCharacteristic?

    private var discoveredPeripherals: [UUID: CBPeripheral] = [:]
    private var discoveredCars: [UUID: DiscoveredCar] = [:] {
        didSet {
            cars.send(discoveredCars.values.sorted { $0.rssi > $1.rssi })
        }
    }
    private var wantsScan = false
    private var pruneTimer: Timer?

    override init() {
        super.init()
        // Delegate callbacks on the main queue keeps all state on one thread
        centralManager = CBCentralManager(delegate: self, queue: .main)
    }

    // MARK: Scanning

    func startScan() {
        wantsScan = true
        discoveredCars = [:]
        discoveredPeripherals = [:]
        beginScanIfReady()

        pruneTimer?.invalidate()
        pruneTimer = Timer.scheduledTimer(withTimeInterval: 1, repeats: true) { [weak self] _ in
            self?.pruneStaleCars()
        }
    }

    func stopScan() {
        wantsScan = false
        pruneTimer?.invalidate()
        pruneTimer = nil
        if centralManager.isScanning {
            centralManager.stopScan()
        }
    }

    private func beginScanIfReady() {
        guard wantsScan, centralManager.state == .poweredOn else { return }
        // Allow duplicates so RSSI stays live and stale cars can be dropped
        centralManager.scanForPeripherals(
            withServices: [SERVICE_UUID],
            options: [CBCentralManagerScanOptionAllowDuplicatesKey: true]
        )
    }

    private func pruneStaleCars() {
        let cutoff = Date().addingTimeInterval(-STALE_CAR_TIMEOUT)
        let stale = discoveredCars.values.filter { $0.lastSeen < cutoff }.map(\.id)
        guard !stale.isEmpty else { return }
        for id in stale {
            discoveredCars[id] = nil
        }
    }

    // MARK: Connecting

    func connect(to car: DiscoveredCar) {
        guard let target = discoveredPeripherals[car.id] else { return }

        // Only one car at a time
        if let current = peripheral, current.identifier != target.identifier {
            centralManager.cancelPeripheralConnection(current)
        }

        peripheral = target
        target.delegate = self
        connectionState.send(.connecting(car))
        centralManager.connect(target, options: nil)
    }

    func disconnect() {
        guard let peripheral = peripheral else { return }
        centralManager.cancelPeripheralConnection(peripheral)
    }

    private func resetConnection() {
        peripheral = nil
        commandChar = nil
        subscribeChar = nil
        connectionState.send(.disconnected)
    }

    func send(command: BLECommand) {
        guard
            case .connected = connectionState.value,
            let peripheral = peripheral,
            let commandChar = commandChar
        else { return }

        let payload = command.payload

        peripheral.writeValue(payload, for: commandChar, type: .withResponse)
    }
}

extension BLEController: CBCentralManagerDelegate {

    func centralManagerDidUpdateState(_ central: CBCentralManager) {
        bluetoothState.send(central.state)

        switch central.state {
        case .poweredOn:
            beginScanIfReady()
        default:
            if peripheral != nil {
                resetConnection()
            }
        }
    }

    func centralManager(_ central: CBCentralManager, didConnect peripheral: CBPeripheral) {
        if let car = connectionState.value.car {
            connectionState.send(.connected(car))
        }
        peripheral.discoverServices([SERVICE_UUID, BATTERY_SERVICE_UUID])
    }

    func centralManager(_ central: CBCentralManager, didFailToConnect peripheral: CBPeripheral, error: (any Error)?) {
        guard peripheral.identifier == self.peripheral?.identifier else { return }
        resetConnection()
    }

    func centralManager(_ central: CBCentralManager, didDisconnectPeripheral peripheral: CBPeripheral, error: (any Error)?) {
        guard peripheral.identifier == self.peripheral?.identifier else { return }
        resetConnection()
    }

    func centralManager(_ central: CBCentralManager, didDiscover peripheral: CBPeripheral, advertisementData: [String : Any], rssi RSSI: NSNumber) {

        // RSSI of 127 means "unavailable"
        let rssi = RSSI.intValue
        guard rssi != 127 else { return }

        let name = advertisementData[CBAdvertisementDataLocalNameKey] as? String
            ?? peripheral.name
            ?? "Unknown Roadster"

        discoveredPeripherals[peripheral.identifier] = peripheral
        discoveredCars[peripheral.identifier] = DiscoveredCar(
            id: peripheral.identifier,
            name: name,
            rssi: rssi,
            lastSeen: Date()
        )
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
                    peripheral.setNotifyValue(true, for: c)
                    // Show the level right away instead of waiting for the next notify
                    peripheral.readValue(for: c)
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
