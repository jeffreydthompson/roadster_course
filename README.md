# Open Roadster

An open source remote control car — mechanical designs, circuit schematics, firmware, and software all in one place.

![Open Roadster Blueprint](mechanical/open%20roadster%20blueprint.png)

## Overview

The Open Roadster is a fully open source RC car project covering every layer of the build:

- **Mechanical** — 3D-printable body and chassis designed in Blender
- **Circuit** — Wiring and component schematics
- **Firmware** — ESP32-S3 motor, steering, and battery monitoring over Bluetooth LE
- **Software** — iOS remote control app (BLE)

## Repository Structure

```
├── mechanical/
│   ├── roadster.blend              # Blender source file
│   ├── open roadster blueprint.png
│   └── blueprints/                 # front, back, side, top views
├── code/
│   ├── app/ios/RoadsterRemote/     # iOS remote control app (SwiftUI + CoreBluetooth)
│   └── firmware/
│       ├── esp-flash.sh            # compile/upload helper (arduino-cli)
│       ├── roadster_ctrl/          # main car firmware
│       └── servo/                  # early servo + motor bench test sketch
└── circuits/                       # (coming soon)
```

## Hardware

| Component | Details |
|-----------|---------|
| Microcontroller | ESP32-S3 |
| Steering | Servo on GPIO 45 (333 Hz, 900–2100 µs, center 1520 µs) |
| Left drive motor | DRV8833 on GPIO 6 / 5 (sleep 11, fault 7) |
| Right drive motor | DRV8833 on GPIO 39 / 40 (sleep 38, fault 34) |
| Fuel gauge | MAX17048 over I2C (SDA 8, SCL 9) |
| Status / headlight LED | GPIO 42 |

All pins are defined at the top of `code/firmware/roadster_ctrl/roadster_ctrl.ino`. Motor direction is corrected in software with `setInverted(true)` in the same file — if a wheel spins the wrong way, that's the line to change.

## Getting Started

### Firmware

1. Install [arduino-cli](https://arduino.github.io/arduino-cli/) (or the [Arduino IDE](https://www.arduino.cc/en/software))
2. Install the ESP32 board support (`esp32:esp32`, tested with 3.3.x)
3. Install the libraries: `arduino-cli lib install ESP32Servo "Adafruit MAX1704X"`
4. Compile and upload:

```sh
cd code/firmware
./esp-flash.sh compile roadster_ctrl
./esp-flash.sh upload roadster_ctrl /dev/cu.usbmodem1101   # your port; see `arduino-cli board list`
```

The board target is set at the top of `esp-flash.sh` (`esp32:esp32:esp32s3`). If you use the Arduino IDE instead, open `code/firmware/roadster_ctrl/roadster_ctrl.ino` and select an ESP32S3 board.

### Driving the car

The car advertises over Bluetooth LE as `Open Roadster` and exposes two services:

| Service | UUID | Purpose |
|---------|------|---------|
| Nordic UART | `6E400001-B5A3-F393-E0A9-E50E24DCCA9E` | Control |
| ↳ RX (write) | `6E400002-B5A3-F393-E0A9-E50E24DCCA9E` | Control packets / commands |
| ↳ TX (notify) | `6E400003-B5A3-F393-E0A9-E50E24DCCA9E` | Feedback for single-character commands |
| Battery | `180F` | Standard Bluetooth battery service |
| ↳ Battery Level | `2A19` | 0–100 %, from the MAX17048 |

**Control packet** (what the iOS app sends) — 5 bytes, little-endian:

| Bytes | Type | Field | Range |
|-------|------|-------|-------|
| 0–1 | `int16` | steering | -100 (left) … 100 (right) |
| 2–3 | `int16` | throttle | -255 (reverse) … 255 (forward) |
| 4 | `uint8` | headlight | 0 = off, 1 = on |

**Single-character commands** — writing one byte to RX also works, which makes it easy to test with a generic BLE UART app (nRF Connect, Bluefruit Connect):

| Command | Action |
|---------|--------|
| `f` | Forward, full speed |
| `b` | Backward, 25% speed |
| `s` | Stop |
| `l` / `r` / `c` | Steer left / right / center |

The same commands work over USB serial (115200 baud) when `ENABLE_SERIAL` is set to `true` in `roadster_ctrl.ino` (it is off by default).

As a safety failsafe, the motors stop automatically if the Bluetooth connection drops.

### iOS App

1. Open `code/app/ios/RoadsterRemote/RoadsterRemote.xcodeproj` in Xcode
2. In the project settings, under **Signing & Capabilities**, select your own **Team** (or create a free Apple Developer account)
3. Update the **Bundle Identifier** (e.g., `com.yourname.RoadsterRemote`) — the placeholder `com.example.RoadsterRemote` won't work
4. Build and run on your device

On launch the app scans for nearby cars and lists every Open Roadster in range with its signal strength. Tap one to connect. Tap the connection indicator any time to switch cars or disconnect. The app drops back to Park whenever the connection is lost.

### Mechanical

1. Open `mechanical/roadster.blend` in [Blender](https://www.blender.org/)
2. Export STL files for 3D printing
3. Refer to the blueprints for assembly dimensions

## License

- Code (`code/`) — [MIT](LICENSE)
- Hardware and mechanical designs (`mechanical/`, `circuits/`) — [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/)

See [LICENSE](LICENSE) for details.
