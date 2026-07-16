//
//  GearView.swift
//  RoadsterRemote
//
//  Created by Jeffrey Thompson on 2/11/26.
//

import SwiftUI

struct GearView: View {
    
    @Binding var gear: Gear
    
    var body: some View {
        VStack {
            Button {
                gear = .park
            } label: {
                label("P", isSelected: gear == .park)
            }
            Button {
                gear = .reverse
            } label: {
                label("R", isSelected: gear == .reverse)
            }
            Button {
                gear = .drive
            } label: {
                label("D", isSelected: gear == .drive)
            }
        }
    }
    
    func label(_ str: String, isSelected: Bool) -> some View {
        ZStack {
            RoundedRectangle(cornerRadius: 10)
                .fill(isSelected ? Color.white : Color.gray)
                .aspectRatio(1.0, contentMode: .fill)
                
            RoundedRectangle(cornerRadius: 10)
                .stroke(lineWidth: 4)
                .foregroundStyle(isSelected ? Color.blue : Color.gray)
                .aspectRatio(1.0, contentMode: .fill)
            Text(str)
                .font(.system(size: 60))
                .foregroundColor(Color.black)
        }
        .aspectRatio(contentMode: .fit)
    }
}

#Preview {
    @Previewable @State var gear: Gear = .park
    GearView(gear: $gear)
}
