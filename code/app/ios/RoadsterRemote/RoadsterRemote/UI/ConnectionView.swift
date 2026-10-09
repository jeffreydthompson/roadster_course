//
//  ConnectionView.swift
//  RoadsterRemote
//
//  Created by Jeffrey Thompson on 2/11/26.
//

import SwiftUI

struct ConnectionView: View {
    
    let connected: Bool
    
    var body: some View {
        HStack {
            Image(systemName: "dot.radiowaves.left.and.right")
                .resizable()
                .aspectRatio(contentMode: .fit)
            led
        }
    }
    
    var led: some View {
        ZStack {
            Circle()
                .fill(Color.gray)
            Circle()
                .scale(0.8)
                .fill(connected ? ledOn : ledOff)
        }
    }
    
    var ledOff: Color {
        Color.init(hue: 0.33, saturation: 0.5, brightness: 0.3)
    }
    
    var ledOn: Color {
        Color.init(hue: 0.33, saturation: 1.0, brightness: 1.0)
    }
}

#Preview {
    ConnectionView(connected: false)
}
