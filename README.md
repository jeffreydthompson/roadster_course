# Open Roadster

An open source remote control car — mechanical designs, circuit schematics, firmware, and software all in one place.

![Open Roadster Blueprint](mechanical/open%20roadster%20blueprint.png)

## Overview

The Open Roadster is a fully open source RC car project covering every layer of the build:

- **Mechanical** — 3D-printable body and chassis designed in Blender
- **Circuit** — Wiring and component schematics
- **Firmware** — ESP32-based motor and servo control
- **Software** — iOS remote control app (BLE)

## Repository Structure

```
├── mechanical/          # 3D models and design files
│   ├── roadster.blend   # Blender source file
│   └── open roadster blueprint.png
├── firmware/            # ESP32 firmware
│   └── servo/           # Servo and motor driver module
├── code/
│   └── app/ios/         # iOS remote control app
└── circuits/            # (coming soon)
```

## Hardware

| Component | Details |
|-----------|---------|
| Microcontroller | ESP32 |
| Steering | Servo on GPIO 25 |
| Drive motor | DC motor via H-bridge on GPIO 32/33 |
| Servo library | [ESP32Servo](https://github.com/madhephaestus/ESP32Servo) |

## Getting Started

### Firmware

1. Install the [Arduino IDE](https://www.arduino.cc/en/software) or [PlatformIO](https://platformio.org/)
2. Install the ESP32 board support
3. Install the `ESP32Servo` library
4. Open `firmware/servo/servo.ino` and upload to your ESP32

### Mechanical

1. Open `mechanical/roadster.blend` in [Blender](https://www.blender.org/)
2. Export STL files for 3D printing
3. Refer to the blueprint for assembly dimensions

### iOS App

1. Open `code/app/ios/RoadsterRemote/RoadsterRemote.xcodeproj` in Xcode
2. In the project settings, under **Signing & Capabilities**, select your own **Team** (or create a free Apple Developer account)
3. Update the **Bundle Identifier** (e.g., `com.yourname.RoadsterRemote`) — the placeholder `com.example.RoadsterRemote` won't work
4. Build and run on your device

## License

This project is open source. See the LICENSE file for details.
