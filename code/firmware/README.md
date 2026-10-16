# Firmware

Arduino firmware for the Open Roadster's ESP32-S3. It drives the rear motors and steering servo, runs the lights, reads the battery, and takes commands from the [app](../app/) over Bluetooth LE.

```
firmware/
├── esp-flash.sh            # compile/upload helper (arduino-cli)
└── roadster_ctrl/          # the car firmware
```

## Building and flashing

1. Install [arduino-cli](https://arduino.github.io/arduino-cli/) (or the [Arduino IDE](https://www.arduino.cc/en/software))
2. Install the ESP32 board support (`esp32:esp32`, tested with 3.3.x)
3. Install the libraries: `arduino-cli lib install ESP32Servo "Adafruit MAX1704X"`
4. Plug the car into your computer over USB-C, then compile and upload:

```sh
cd code/firmware
./esp-flash.sh compile roadster_ctrl
./esp-flash.sh upload roadster_ctrl /dev/cu.usbmodem1101   # your port; see `arduino-cli board list`
```

The board target is set at the top of `esp-flash.sh` (`esp32:esp32:esp32s3`). If you use the Arduino IDE instead, open `roadster_ctrl/roadster_ctrl.ino` and select an ESP32S3 board.

## Pins

| Function | Pins |
|-----------|---------|
| Steering | Servo on GPIO 45 (333 Hz, 900–2100 µs, center 1520 µs) |
| Left drive motor | DRV8833 on GPIO 6 / 5 (sleep 11, fault 7) |
| Right drive motor | DRV8833 on GPIO 39 / 40 (sleep 38, fault 34) |
| Fuel gauge | MAX17048 over I2C (SDA 8, SCL 9) |
| Front light (headlights, `F.LED`) | GPIO 42, PWM |
| Rear light (tail / brake, `B.LED`) | GPIO 41, PWM |
| Power source (TPS2116 `ST`) | GPIO 26: HIGH on USB power, LOW on battery |

All pins are defined at the top of `roadster_ctrl/roadster_ctrl.ino`. Motor direction is corrected in software with `setInverted(true)` in the same file. If a wheel spins the wrong way, that's the line to change.

## Bluetooth interface

The car advertises over Bluetooth LE as `Open Roadster` and exposes two services:

| Service | UUID | Purpose |
|---------|------|---------|
| Nordic UART | `6E400001-B5A3-F393-E0A9-E50E24DCCA9E` | Control |
| ↳ RX (write) | `6E400002-B5A3-F393-E0A9-E50E24DCCA9E` | Control packets / commands |
| ↳ TX (notify) | `6E400003-B5A3-F393-E0A9-E50E24DCCA9E` | Feedback for single-character commands |
| Battery | `180F` | Standard Bluetooth battery service |
| ↳ Battery Level | `2A19` | 0–100 %, from the MAX17048 |
| ↳ Power Source | `8FDDF80E-8D10-4B50-9466-FCE56AF3B124` | 1 = on USB power (battery charging), 0 = on battery; notifies on change |

### Control packet

This is what the app sends: 5 bytes, little-endian.

| Bytes | Type | Field | Range |
|-------|------|-------|-------|
| 0–1 | `int16` | steering | -100 (left) … 100 (right) |
| 2–3 | `int16` | throttle | -255 (reverse) … 255 (forward), motor PWM |
| 4 | `uint8` | headlight | 0 = off, 1 = on |

### Single-character commands

Writing one byte to RX also works. That makes it easy to test with a generic BLE UART app (nRF Connect, Bluefruit Connect):

| Command | Action |
|---------|--------|
| `f` | Forward, full speed |
| `b` | Backward, 25% speed |
| `s` | Stop |
| `l` / `r` / `c` | Steer left / right / center |

The same commands work over USB serial (115200 baud) when `ENABLE_SERIAL` is set to `true` in `roadster_ctrl.ino`. It's off by default.

As a safety failsafe, the motors stop automatically if the Bluetooth connection drops.

## Lights

The headlight flag in the control packet turns on the front light, and the rear light glows as a tail light.

The rear light goes to full brightness as a brake light when the throttle is released or drops sharply, and holds for 0.8 s. It also stays on while reversing.

Brightness levels and timing are constants at the top of `LightsModule.h`.
