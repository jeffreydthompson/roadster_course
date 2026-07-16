//
//  Accelerometer.swift
//  RoadsterRemote
//
//  Created by Jeffrey Thompson on 2/8/26.
//

import CoreMotion
import Combine

class Accelerometer {
    
    let motionManager = CMMotionManager()
    
    var publisher = PassthroughSubject<CMAttitude, Error>()
    
    init() { }
    
    func beginUpdates() {
        motionManager.deviceMotionUpdateInterval = 0.1
        motionManager.startDeviceMotionUpdates(to: .main) {[weak self] motion, error in
            
            if let error = error {
                self?.publisher.send(completion: .failure(error))
                return
            }
            
            if let motion = motion {
                self?.publisher.send(motion.attitude)
            }
        }
    }
    
}
