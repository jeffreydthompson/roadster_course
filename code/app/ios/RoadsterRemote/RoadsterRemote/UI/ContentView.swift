//
//  ContentView.swift
//  RoadsterRemote
//
//  Created by Jeffrey Thompson on 2/7/26.
//

import SwiftUI

struct ContentView: View {
    
    @State var controller = CentralControl()
    @State private var showingCarPicker = true
    
    let grad: LinearGradient = .init(gradient: .init(colors: [.init(white: 0.2), .init(white: 0.4)]), startPoint: .bottom, endPoint: .top)
    
    var body: some View {
        ZStack {
            grad
                .ignoresSafeArea()
            HStack {
                GearView(gear: $controller.gear)
                
                VStack {
                    FuelGaugeView(batteryLevel: controller.batteryLevel, isCharging: controller.isOnUsbPower)
                    
                    HStack {
                        HeadlightControlView(isOn: $controller.headlight)
                            .padding()
                        Button {
                            showingCarPicker = true
                        } label: {
                            ConnectionView(connected: controller.isConnected)
                        }
                        .accessibilityLabel(controller.isConnected ? "Connected. Change car" : "Not connected. Select car")
                        .padding()
                    }
                    .padding(15)
                }
                .padding(15)
                .foregroundStyle(Color.white)
                
                ThrottleView(throttle: $controller.throttle)
                    .frame(width: 165)
                    .clipShape(RoundedRectangle(cornerRadius: 25))
            }
            .padding(15)
        }
        .sheet(isPresented: $showingCarPicker) {
            CarPickerView(controller: controller)
        }
        /*ZStack {
            grad
                .ignoresSafeArea()
            FuelGaugeView(batteryLevel: controller.batteryLevel)
                .foregroundStyle(Color.white)
                .frame(width: 300, height: 300)
                .offset(x: -40, y: -70)
            ConnectionView(connected: controller.isConnected)
                .foregroundStyle(Color.white)
                .frame(width: 250)
                .offset(x: -40, y: 120)
                .shadow(radius: 5)
            HStack {
                GearView(gear: $controller.gear)
                    .padding(25)
                Spacer()
                ThrottleView(throttle: $controller.throttle)
                    .clipShape(RoundedRectangle(cornerRadius: 45))
                    .frame(width: 175)
                    .padding()
                    .shadow(radius: 5)
            }
        }*/
    }
}

#Preview {
    ContentView()
}
