//
//  HeadlightControlView.swift
//  RoadsterRemote
//
//  Created by Jeffrey Thompson on 2/14/26.
//

import SwiftUI

struct HeadlightControlView: View {
    
    @Binding var isOn: Bool
    
    var body: some View {
        Button {
            isOn = !isOn
        } label: {
            Image(systemName: imageName)
                .resizable()
                .aspectRatio(contentMode: .fit)
                .foregroundStyle(color)
        }
    }
    
    var color: Color {
        isOn ? .yellow : .gray
    }
    
    var imageName: String {
        isOn ? "headlight.high.beam.fill" : "headlight.high.beam"
    }
}

#Preview {
    @Previewable @State var isOn = true
    
    HeadlightControlView(isOn: $isOn)
}
