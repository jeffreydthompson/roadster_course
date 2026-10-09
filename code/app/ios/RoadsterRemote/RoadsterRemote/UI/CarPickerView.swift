//
//  CarPickerView.swift
//  RoadsterRemote
//
//  Scan for nearby Roadsters and pick one to drive.
//

import SwiftUI
import CoreBluetooth

struct CarPickerView: View {

    let controller: CentralControl
    @Environment(\.dismiss) private var dismiss

    var body: some View {
        NavigationStack {
            content
                .navigationTitle("Select Your Roadster")
                .navigationBarTitleDisplayMode(.inline)
                .toolbar {
                    ToolbarItem(placement: .confirmationAction) {
                        Button("Done") { dismiss() }
                    }
                }
        }
        .onAppear { controller.startScan() }
        .onDisappear { controller.stopScan() }
        .onChange(of: controller.connectionState) { _, state in
            // Picked a car and it connected - get out of the way
            if case .connected = state {
                dismiss()
            }
        }
    }

    @ViewBuilder
    var content: some View {
        if controller.bluetoothState == .poweredOff {
            ContentUnavailableView(
                "Bluetooth Is Off",
                systemImage: "antenna.radiowaves.left.and.right.slash",
                description: Text("Turn on Bluetooth in Settings to find your Roadster.")
            )
        } else if controller.bluetoothState == .unauthorized {
            ContentUnavailableView(
                "Bluetooth Access Needed",
                systemImage: "lock",
                description: Text("Allow Bluetooth for Roadster Remote in Settings.")
            )
        } else {
            List {
                if let car = controller.connectionState.car {
                    Section("Current") {
                        currentCarRow(car)
                    }
                }

                Section {
                    if nearbyCars.isEmpty {
                        HStack(spacing: 12) {
                            ProgressView()
                            Text("Make sure your Roadster is powered on and nearby.")
                                .foregroundStyle(.secondary)
                        }
                        .padding(.vertical, 6)
                    }
                    ForEach(nearbyCars) { car in
                        Button {
                            controller.connect(to: car)
                        } label: {
                            CarRow(car: car)
                        }
                        .disabled(isConnecting)
                    }
                } header: {
                    HStack {
                        Text("Nearby")
                        Spacer()
                        ProgressView()
                            .controlSize(.mini)
                    }
                }
            }
        }
    }

    // The connected car stops advertising, so it's shown separately
    private var nearbyCars: [DiscoveredCar] {
        controller.cars.filter { $0.id != controller.connectionState.car?.id }
    }

    private var isConnecting: Bool {
        if case .connecting = controller.connectionState { return true }
        return false
    }

    private func currentCarRow(_ car: DiscoveredCar) -> some View {
        HStack {
            CarRow(car: car, showSignal: false)
            Spacer()
            if isConnecting {
                ProgressView()
                Text("Connecting…")
                    .foregroundStyle(.secondary)
            } else {
                Image(systemName: "checkmark.circle.fill")
                    .foregroundStyle(.green)
            }
            Button(isConnecting ? "Cancel" : "Disconnect", role: .destructive) {
                controller.disconnect()
            }
            .buttonStyle(.bordered)
            .padding(.leading, 8)
        }
    }
}

fileprivate struct CarRow: View {

    let car: DiscoveredCar
    var showSignal = true

    var body: some View {
        HStack(spacing: 12) {
            Image(systemName: "car.side.fill")
                .font(.title2)
                .foregroundStyle(.tint)
            VStack(alignment: .leading) {
                Text(car.name)
                    .foregroundStyle(.primary)
                Text("ID \(car.shortID)")
                    .font(.caption)
                    .monospaced()
                    .foregroundStyle(.secondary)
            }
            if showSignal {
                Spacer()
                Image(systemName: "cellularbars", variableValue: car.signalStrength)
                    .foregroundStyle(.secondary)
                    .accessibilityLabel("Signal strength \(Int(car.signalStrength * 100)) percent")
            }
        }
    }
}

#Preview {
    CarPickerView(controller: CentralControl())
}
