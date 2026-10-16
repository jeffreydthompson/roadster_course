# Code

| Folder | What it is |
|---|---|
| [firmware/](firmware/) | Arduino firmware for the car's ESP32-S3: motors, steering, lights, battery, Bluetooth LE |
| [app/](app/) | iOS remote control app (SwiftUI + CoreBluetooth) |

The two talk over Bluetooth LE. The protocol is documented in the [firmware README](firmware/README.md#bluetooth-interface).
