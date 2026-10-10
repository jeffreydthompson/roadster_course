# TODO

## Battery protection

A pack left with the power switch on drains until the buck-boost gives up
(about 1.8 V), which kills the cell. Nothing on the Rev 2 board stops this.

### Firmware (Rev 2 boards)

- [ ] Low-battery cutoff: poll the MAX17048 cell voltage (already read every
      2 s in `BatteryModule`).
  - Below ~3.3 V under load: stop the motors and flash the lights as a warning.
  - Below ~3.2 V: stop the motors and put the ESP32 into deep sleep.
  - This is only partial protection. The servo, fuel gauge, regulator and
    motor drivers stay powered in sleep (estimated 1–5 mA, not yet measured),
    so it buys days, not months.
- [ ] Measure the actual idle current with the switch on (advertising, and in
      deep sleep) to know how long a forgotten car lasts.
- [ ] Report cell voltage over BLE alongside the percentage, since the
      MAX17048 % is unreliable until it has seen a charge/discharge cycle.

### Hardware (Rev 3 board)

- [ ] Add a battery protection IC (e.g. DW01A + FS8205A) between the
      BATTERY connector and VBATT, for hardware over-discharge, over-charge
      and short-circuit cutoff.
- [ ] Connect MAX17048 ALRT# to a GPIO (it's unconnected on Rev 2) so a low
      battery alert can wake the ESP32.
- [ ] Consider letting the ESP32 shut off the TPS63001 (its EN is currently
      tied to VIN through R4) or switch the servo supply, so firmware can do
      a real cutoff.

The power switch (SW2) itself is fine. It disconnects everything except the
MCP73831's VBAT pin (~0.25 µA) and the reverse-polarity FET, and charging
still works with it off.

## App / firmware

- [ ] Throttle range mismatch: the iOS app sends 0..100 with no reverse,
      the firmware expects −255..255. App full throttle is only ~40%.

## PCB (KiCad)

- [ ] Remaining DRC items: courtyard overlaps, CHARGED/CHARGING silk over SW2.
- [ ] 3D models for the footprints.
