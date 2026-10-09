//
//  FuelGaugeView.swift
//  RoadsterRemote
//
//  Created by Jeffrey Thompson on 2/11/26.
//

import SwiftUI

struct FuelGaugeView: View {
    
    let batteryLevel: UInt8
    
    var body: some View {
        
        GeometryReader { geo in
            
            let w = geo.size.width
            let h = geo.size.height
            
            let fontSize = h/5.0
            let lineWeight = h / 10.0
            let yOffset = (w/2) - (w/10)
            
            ZStack {
                Text("F")
                    .bold()
                    .font(.system(size: fontSize))
                    .offset(offsetByPercentage(geo, xP: 0.43, yP: 0.27))
                Text("E")
                    .bold()
                    .font(.system(size: fontSize))
                    .offset(offsetByPercentage(geo, xP: -0.43, yP: 0.27))
                // ticks
                Group {
                    Arc(startAngle: .degrees(310), endAngle: .degrees(312), clockwise: false)
                        .stroke(lineWidth: lineWeight * 3)
                        .fill(Color.white)
                        .offset(x: 0, y: yOffset)
                    Arc(startAngle: .degrees(334), endAngle: .degrees(336), clockwise: false)
                        .stroke(lineWidth: lineWeight * 2)
                        .fill(Color.white)
                        .offset(x: 0, y: yOffset)
                    Arc(startAngle: .degrees(359), endAngle: .degrees(1), clockwise: false)
                        .stroke(lineWidth: lineWeight * 3)
                        .fill(Color.white)
                        .offset(x: 0, y: yOffset)
                    Arc(startAngle: .degrees(24), endAngle: .degrees(26), clockwise: false)
                        .stroke(lineWidth: lineWeight * 2)
                        .fill(Color.white)
                        .offset(x: 0, y: yOffset)
                    Arc(startAngle: .degrees(48), endAngle: .degrees(50), clockwise: false)
                        .stroke(lineWidth: lineWeight * 3)
                        .fill(Color.white)
                        .offset(x: 0, y: yOffset)
                }
                Arc(startAngle: .degrees(310), endAngle: .degrees(50), clockwise: false)
                    .stroke(lineWidth: lineWeight)
                    .fill(Color.gray)
                    .offset(x: 0, y: yOffset)
                Arc(startAngle: .degrees(310), endAngle: .degrees(310 + Double(batteryLevel)), clockwise: false)
                    .stroke(lineWidth: lineWeight)
                    .fill(gaugeColor())
                    .offset(x: 0, y: yOffset)
            }
        }
    }
    
    func offsetByPercentage(_ geo: GeometryProxy, xP: CGFloat, yP: CGFloat) -> CGSize {

        let pX = geo.size.width * xP
        let pY = geo.size.height * yP
        
        return .init(width: pX, height: pY)
    }
    
    var path: some View {
        GeometryReader { geo in
            
            let h = geo.size.height
            let w = geo.size.width
            let p1 = CGPoint(x: 0, y: 0)
            let p2 = CGPoint(x: w, y: h)
            
            var path = Path()
            
            path.addArc(tangent1End: p1, tangent2End: p2, radius: 350)
            
            return path
        }
    }
    
    func gaugeColor() -> Color {
        let hue = Double(batteryLevel) / 300.0
        return Color.init(hue: hue, saturation: 1.0, brightness: 1.0)
    }
}

fileprivate struct Arc: Shape {
    
    var startAngle: Angle
    var endAngle: Angle
    var clockwise: Bool = false

    func path(in rect: CGRect) -> Path {

        let radius = rect.width / 2
        let center = CGPoint(x: rect.midX, y: rect.midY)

        var path = Path()

        path.addArc(
            center: center,
            radius: radius,
            startAngle: startAngle - .degrees(90),
            endAngle: endAngle - .degrees(90),
            clockwise: clockwise
        )

        return path
    }
}

#Preview {
    VStack {
        FuelGaugeView(batteryLevel: 14)
    }
}
