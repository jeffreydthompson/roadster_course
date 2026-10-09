//
//  ThrottleView.swift
//  RoadsterRemote
//
//  Created by Jeffrey Thompson on 2/8/26.
//

import SwiftUI

struct ThrottleView: View {
    
    @Binding public var throttle: Int16
    @State private var gesturePos: CGFloat = 0
    
    private var gradient: LinearGradient {
        LinearGradient(gradient: Gradient(colors: [.init(white: 0.5), .init(white: 0.7), .init(white: 0.8), .init(white: 0.7)]), startPoint: .bottom, endPoint: .top)
    }
    
    var body: some View {
        GeometryReader { geo in
            
            let h = geo.size.height
            let w = geo.size.width
            
            let jStickPad = w / 10
            let jStickDim = w - (jStickPad * 2)
            let jStickDimCenter = (w / 2) - jStickPad
            
            let jStickYCenter = (h/2) - (w/2)
            let jStickMaxY = jStickYCenter + ((h - jStickDim)/2)
            let jOffsetRange = (CGFloat(throttle) / 100) * (h - jStickDim)
            let jOffset = jOffsetRange
            
            // jOffset
            // map 0 -> h to (0 + jStickCenter) -> (h - jStickCenter)
            
//            let jOffset = (h - w) - joystickPos
            
            
            ZStack {
                gradient
                Rectangle()
                    .fill(Color.init(white: 0.15))
                    .scaleEffect(x: 0.15, y: 0.9)
                JoyStick()
                    .padding(jStickPad)
                    .offset(x: 0, y: jStickMaxY - jOffset)
            }
                .gesture(
                    DragGesture(minimumDistance: 0)
                        .onChanged {
                            
                            let y = $0.location.y
                            let dY = h - y
                            
                            gesturePos = dY
                            gesturePos = max(gesturePos, 0+jStickDimCenter)
                            gesturePos = min(gesturePos, h-jStickDimCenter)
                            
                            let mappingH = h - (jStickDimCenter*2)
                            
                            let mapped = mapRange(gesturePos - jStickDimCenter, 0, mappingH, 0, 100)
                            
//                            let yNorm = (gesturePos / (h - (jStickDimCenter))) * 100

                            throttle = Int16(mapped)
                        }
                        .onEnded { _ in
                            throttle = 0
                        }
                )
        }
    }
}

fileprivate struct JoyStick: View {
    
    var body: some View {
        GeometryReader { geo in
            
            let w = geo.size.width
            let pad = w / 10
            
            let backGrad: RadialGradient = .init(colors: [.black, .init(white: 0.9)], center: .center, startRadius: 0, endRadius: w)
            let frontGrad: RadialGradient = .init(colors: [.gray, .init(white: 0.9)], center: .center, startRadius: 0, endRadius: w)
            
            ZStack {
                Circle()
                    .fill(backGrad)
                Circle()
                    .fill(frontGrad)
                    .padding(pad)
            }
        }
    }
}

// ie C -> F degrees conversion
// 0, 0, 100, 32, 212 -> 32
fileprivate func mapRange(
    _ value: Double,
    _ from1: Double,
    _ to1: Double,
    _ from2: Double,
    _ to2: Double
) -> Double {
    // ratio
    let ratio = (to2 - from2) / (to1 - from1)
    let scaledVal = ratio * value
    let offsetVal = scaledVal + from2
    return offsetVal
    //return ((value - from1) / (to1 - from1)) * (to2 - from2) + from2
}

#Preview {
    @Previewable @State var throttle: Int16 = 0 {
        didSet {
            print(throttle)
        }
    }
//    JoyStick()
    HStack {
        Spacer(minLength: 550)
        ThrottleView(throttle: $throttle)
    }
    .padding()
}
