//
//  CentralControl.swift
//  RoadsterRemote
//
//  Created by Jeffrey Thompson on 2/7/26.
//

import SwiftUI
import Combine

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
    var isConnected: Bool = false
    var throttle: Int16 = 0
    var steering: Int16 = 0 {
        didSet {
            sendCommand()
        }
    }
    var headlight: Bool = false
    
    let bleController: BLEController
    let accelerometer: Accelerometer
    
    var subscribers: Set<AnyCancellable> = []
    
    init() {
        self.bleController = BLEController()
        self.accelerometer = Accelerometer()
        
        self.bleController.batteryLevel
            .receive(on: DispatchQueue.main)
            .sink { [weak self] battery in
                self?.batteryLevel = battery
            }
            .store(in: &subscribers)
        
        self.bleController.connected
            .receive(on: DispatchQueue.main)
            .sink { [weak self] connection in
                self?.isConnected = connection
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
    }
    
    func sendCommand() {
        switch gear {
        case .park:
            break
        case .reverse:
            let rThrottle = -throttle
            let cmd = BLECommand(throttle: rThrottle, steering: steering, headlight: headlight ? 1 : 0)
            bleController.send(command: cmd)
        case .drive:
            let cmd = BLECommand(throttle: throttle, steering: steering, headlight: headlight ? 1 : 0)
            bleController.send(command: cmd)
        }
    }
}
