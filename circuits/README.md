# Circuits

The Open Roadster's controller: a 4-layer, 60 × 40 mm board built around an **ESP32-S3-MINI-1-N8**. It drives the motors, steering and lights, charges and monitors the battery, and talks to the phone over Bluetooth LE.

| Function | Part |
|---|---|
| Microcontroller (Wi-Fi + BLE) | ESP32-S3-MINI-1-N8 |
| Motor drivers (one per rear wheel) | 2× DRV8833 |
| Battery charger (single-cell LiPo, via USB-C) | MCP73831 |
| Fuel gauge | MAX17048 |
| 3.3 V supply (buck-boost) | TPS63001 |
| USB / battery power switchover | TPS2116 |
| USB-to-serial for programming | CP2102N |
| Connectors | USB-C, battery (PH2.0), 2× motor, servo, front + rear LED |

## What's here

```
circuits/
├── RCcar_schematic.pdf     # all schematic sheets, no software needed
├── kicad/                  # KiCad 10 project (schematic + PCB), where the design is maintained
├── fabrication/            # Gerbers, BOM, pick-and-place for JLCPCB (from KiCad)
└── easyeda/                # original EasyEDA Standard source (schematic + PCB)
    └── fabrication/        # archived order files exported from EasyEDA
```

- **Just looking?** Open `RCcar_schematic.pdf`. It has every sheet: Sheet 1, Power, Motor Control, LED Drivers, USB Programmer.
- **Editing the design?** Open `kicad/RCcar.kicad_pro` in [KiCad](https://www.kicad.org/) 10.
- **Ordering boards?** Use the files in `fabrication/` (see below).
- **Prefer EasyEDA?** In EasyEDA Standard, use **File → Open → EasyEDA Source** to open `easyeda/RCcar_schematic.json` and `easyeda/RCcar_pcb.json`.

## Ordering from JLCPCB

`fabrication/` has the order files in JLCPCB's format:

- `RCcar_kicad_gerber.zip`: upload this for bare boards (4 layers, 60 × 40 mm)
- `RCcar_bom_jlcpcb.csv` and `RCcar_cpl_jlcpcb.csv`: add these for assembly. Every part has an LCSC part number

Parts are on both sides, so choose two-sided assembly. Always check part rotations in JLCPCB's assembly preview before paying.

The original boards were made from the EasyEDA order files in `easyeda/fabrication/`, which are kept for reference. The KiCad set was checked against them:

- the same 91 parts with the same LCSC numbers
- identical positions, sides and rotations
- copper within about 1% per layer (pour edges)

KiCad also opens the solder mask around the bare mounting and locating holes.

## About the KiCad port

The design started in EasyEDA. The KiCad project was imported from the EasyEDA source and checked against it:

- Schematic and board agree on every connection. KiCad's schematic parity check finds no split or merged nets; the remaining parity notes are naming differences from the import.
- Design rules match EasyEDA's (0.152 mm minimum clearance, 0.254 mm tracks). DRC reports no clearance errors and no unconnected items.
- KiCad fills copper pours slightly differently from EasyEDA, so 14 short tracks (0.2 mm) were added where KiCad's fill couldn't reach a few fine-pitch pins that EasyEDA's pour connected (around U1, U3, U5, U6, U7 and R2).
- KiCad's Gerbers match EasyEDA's copper to within about 1% per layer. The differences are pour edges and thermal spokes around pads.
- The project is self-contained. All symbols and footprints live in project libraries (`RCcar.kicad_sym`, `RCcar.pretty/`), registered in the project's `sym-lib-table` and `fp-lib-table`.
- Remaining DRC/ERC items are left visible on purpose. They are cosmetic or come from the import:
  - courtyard overlaps from the tight placement
  - EasyEDA silkscreen touching pads
  - footprints that differ from their library copy by rounding or 3D model
  - EasyEDA symbols whose pin types trigger "not driven" and pin-type warnings
- 3D models aren't included yet, so the 3D viewer shows bare footprints.

The original EasyEDA board passes EasyEDA's design rule check with no errors, and all nets are routed.

## Known limitations

The board has no battery protection IC, so a pack left switched on can be run flat. See [TODO.md](../TODO.md) for the plan.
