# App

The remote control for the Open Roadster. It's currently iOS only (SwiftUI + CoreBluetooth), in `ios/RoadsterRemote/`.

## Driving

Hold the phone in landscape.

- **Steering:** tilt the phone like a steering wheel.
- **Throttle:** slide the stick on the right.
- **Gear selector (P / R / D):** sets Park, Reverse or Drive.
- **Headlights:** toggle them on and off.
- **Fuel gauge:** shows the car's battery level and whether it's charging.

On launch the app scans for nearby cars and lists every Open Roadster in range with its signal strength. Tap one to connect. Tap the connection indicator any time to switch cars or disconnect.

If the connection drops (the car goes out of range or is switched off), the app shifts to Park and goes back to the car list. The car shows up under Nearby again once it's back in range.

## Building

1. Open `ios/RoadsterRemote/RoadsterRemote.xcodeproj` in Xcode
2. In the project settings, under **Signing & Capabilities**, select your own **Team** (or create a free Apple Developer account)
3. Change the **Bundle Identifier** to one of your own (e.g., `com.yourname.RoadsterRemote`). Bundle IDs are unique per Apple account, so the default `org.openroadster.RoadsterRemote` may already be taken
4. Build and run on your device. It needs iOS 18 or newer

The app needs Bluetooth to find the car and motion sensing for tilt steering. iOS asks for both on first launch.

## Protocol

The app sends a 5-byte control packet (steering, throttle, headlight) to the car's Nordic UART service. It reads the standard Battery Level characteristic and a Power Source characteristic for the charging indicator. The full protocol is in the [firmware README](../firmware/README.md#bluetooth-interface), which is useful if you want to write your own controller.
