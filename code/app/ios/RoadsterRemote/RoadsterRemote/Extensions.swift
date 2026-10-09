//
//  Extensions.swift
//  RoadsterRemote
//
//  Created by Jeffrey Thompson on 2/8/26.
//

import Foundation

extension Data {
    var hexSpacedUpper: String {
        map { String(format: "%02X", $0) }.joined(separator: " ")
    }
}
