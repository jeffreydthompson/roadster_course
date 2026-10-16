# Open Roadster

An open source remote control car you can build from scratch. The chassis, circuit board, firmware, and phone app are all here.

![Open Roadster Blueprint](mechanical/open%20roadster%20blueprint.png)

## What's inside

| | Folder | What you'll find |
|---|---|---|
| **Mechanical** | [`mechanical/`](mechanical/) | Chassis, steering and wheels in FreeCAD, ready-to-print STLs, a sheet metal base, and a Blender body |
| **Circuits** | [`circuits/`](circuits/) | A 4-layer ESP32-S3 controller board in KiCad, with order-ready files for JLCPCB |
| **Firmware** | [`code/firmware/`](code/firmware/) | Arduino firmware for motors, steering, lights, battery monitoring and Bluetooth LE |
| **App** | [`code/app/`](code/app/) | An iOS remote: tilt to steer, with throttle, gears, headlights and a fuel gauge |

Each folder has its own README with the details.

## The car at a glance

- **Brains:** ESP32-S3 on a custom 60 × 40 mm board
- **Drive:** two N20 gear motors, one per rear wheel
- **Steering:** micro servo with a steering linkage
- **Power:** single-cell LiPo, charged over USB-C, with a fuel gauge
- **Lights:** headlights, plus tail lights that double as brake lights
- **Control:** Bluetooth LE from your phone

## Building one

1. **Order the circuit board.** See [circuits](circuits/).
2. **Make the parts:** print the STLs, have the sheet metal base cut, and buy the motors, servo and bearings. See [mechanical](mechanical/).
3. **Flash the firmware** over USB-C. See [firmware](code/firmware/).
4. **Install the app** on your phone and drive. See [app](code/app/).

## Next steps

**A soft power switch.** Right now the car is turned on and off with a plain mechanical power switch. If it's left on, it keeps drawing current until the LiPo is run flat. Over-discharging a LiPo ruins it, and a damaged cell is a fire risk the next time it's charged.

The next board revision will add a soft switch: a push button that turns the car on, with power held on by the microcontroller. The firmware can then cut power completely, for example after a period of inactivity or when the battery gets low. That way the car can never be accidentally left on to drain the battery.

Other work in progress and known issues are tracked in [TODO.md](TODO.md).

## License

- Code (`code/`): [MIT](LICENSE)
- Hardware and mechanical designs (`mechanical/`, `circuits/`): [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/)

See [LICENSE](LICENSE) for details.

The [Open Source Hardware logo](https://oshwa.org/resources/open-source-hardware-logo/) was designed by Macklin Chaffee and is used under a CC-SA license. Open Roadster is not OSHWA certified.
