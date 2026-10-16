//
//  CentralControl.swift
//  RoadsterRemote
//
//  Created by Jeffrey Thompson on 2/7/26.
//

import SwiftUI
import Combine
import CoreBluetooth

// Control packets go out continuously at this rate. The car stops its motors
// if they stop arriving for 500 ms, so keep this well above 2 Hz.
fileprivate let COMMAND_INTERVAL: TimeInterval = 1.0 / 20.0

enum Gear {
    case park, reverse, drive
}

@Observable
public class CentralControl {
    
    var gear: Gear = .park
    
    var pitch: Double = 0 {
        didSet {
            let p = pitch * -100
            let pInt: Int16 = Int16(p)
            self.steering = pInt
        }
    }
    
    var batteryLevel: UInt8 = 0
    var isOnUsbPower: Bool = false
    var connectionState: ConnectionState = .disconnected
    var cars: [DiscoveredCar] = []
    var bluetoothState: CBManagerState = .unknown
    
    var isConnected: Bool {
        if case .connected = connectionState { return true }
        return false
    }
    
    var throttle: Int16 = 0
    var steering: Int16 = 0
    var headlight: Bool = false
    
    let bleController: BLEController
    let accelerometer: Accelerometer
    
    var subscribers: Set<AnyCancellable> = []
    private var commandTimer: Timer?
    
    init() {
        self.bleController = BLEController()
        self.accelerometer = Accelerometer()
        
        self.bleController.batteryLevel
            .receive(on: DispatchQueue.main)
            .sink { [weak self] battery in
                self?.batteryLevel = battery
            }
            .store(in: &subscribers)
        
        self.bleController.onUsbPower
            .receive(on: DispatchQueue.main)
            .sink { [weak self] onUsb in
                self?.isOnUsbPower = onUsb
            }
            .store(in: &subscribers)
        
        self.bleController.connectionState
            .receive(on: DispatchQueue.main)
            .sink { [weak self] state in
                self?.connectionState = state
                if case .connected = state { return }
                // Safety: never resume driving on reconnect
                self?.gear = .park
            }
            .store(in: &subscribers)
        
        self.bleController.cars
            .receive(on: DispatchQueue.main)
            .sink { [weak self] cars in
                self?.cars = cars
            }
            .store(in: &subscribers)
        
        self.bleController.bluetoothState
            .receive(on: DispatchQueue.main)
            .sink { [weak self] state in
                self?.bluetoothState = state
            }
            .store(in: &subscribers)
        
        self.accelerometer.publisher
            .receive(on: DispatchQueue.main)
            .sink { completion in
                switch completion { case .finished:
                    break
                case .failure(let error):
                    debugPrint("ERROR: \(error.localizedDescription)")
                }
            } receiveValue: {[weak self] attitude in
                self?.pitch = attitude.pitch
            }
            .store(in: &subscribers)

        self.accelerometer.beginUpdates()

        let timer = Timer(timeInterval: COMMAND_INTERVAL, repeats: true) { [weak self] _ in
            self?.sendCommand()
        }
        // .common keeps it firing while a gesture is tracking
        RunLoop.main.add(timer, forMode: .common)
        commandTimer = timer
    }
    
    func startScan() {
        bleController.startScan()
    }
    
    func stopScan() {
        bleController.stopScan()
    }
    
    func connect(to car: DiscoveredCar) {
        bleController.connect(to: car)
    }
    
    func disconnect() {
        bleController.disconnect()
    }
    
    func sendCommand() {
        guard isConnected else { return }

        let cmdThrottle: Int16
        switch gear {
        case .park:
            // Keep sending in Park: it's what stops the car, and headlights still work
            cmdThrottle = 0
        case .reverse:
            cmdThrottle = -motorThrottle
        case .drive:
            cmdThrottle = motorThrottle
        }

        let cmd = BLECommand(throttle: cmdThrottle, steering: steering, headlight: headlight ? 1 : 0)
        bleController.send(command: cmd)
    }

    // The throttle stick runs 0...100; the firmware expects motor PWM, 0...255
    private var motorThrottle: Int16 {
        Int16(Int(throttle) * 255 / 100)
    }
}
