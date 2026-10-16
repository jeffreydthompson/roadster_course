# Mechanical

The chassis, steering, wheels and body of the Open Roadster.

![Open Roadster Blueprint](open%20roadster%20blueprint.png)

## What's here

```
mechanical/
├── freecad/
│   ├── OpenRoadster.FCStd      # full chassis assembly, parts in position
│   ├── Printed Parts/          # one file per 3D-printed part
│   ├── Sheet Metal Parts/      # chassis base: model, flat-pattern DXF, bent STEP
│   └── Purchased Parts/        # motors, servo, bearing, screws, PCB
├── stl/                        # ready-to-print STLs of the printed parts
├── roadster.blend              # Blender body model
├── open roadster blueprint.png
└── blueprints/                 # front, back, side, top views
```

## Chassis, steering, and wheels

These are in [FreeCAD](https://www.freecad.org/) (1.0 or newer) under `freecad/`:

- `OpenRoadster.FCStd`: the complete chassis assembly with every part in position
- `Printed Parts/`: one file per printed part, for editing
- `Sheet Metal Parts/`: the chassis base, made by a sheet metal shop rather than printed (see below)
- `Purchased Parts/`: models of the off-the-shelf components, for fit checks

The parts were converted from the original Onshape model as solid bodies, so they don't carry the parametric feature history. They can be measured, modified, and re-exported, but not edited by changing the original sketches.

## 3D printing

Ready-to-print STLs are in `stl/`, exported at 0.01 mm tolerance, in millimetres. To change a part, edit its file in `freecad/Printed Parts/` and export a new STL.

## Sheet metal chassis base

The base (`or.sheetmetal.base`) is 0.8 mm steel: a flat blank of 100.6 × 167.0 mm with 9 bends. Send a fabrication service (SendCutSend, OSH Cut, JLCCNC, etc.) both files from `freecad/Sheet Metal Parts/`:

- `or.sheetmetal.base.flat.dxf`: the flat pattern in mm; outline and holes on layer `CUT`, bend lines on layer `BEND`
- `or.sheetmetal.base.step`: the bent part, so the shop can confirm bend directions and angles

## Purchased parts

| Part | Qty |
|---|---|
| N20 DC gear motor (`or.n20.dc.motor`) | 2 |
| D1802MG micro servo (`or.d1802mg.servo`) | 1 |
| Front wheel bearing (`or.bearing.front`) | 2 |
| Hub screw (`or.hubscrew`) | 4 |
| Controller PCB (`PCB`), see [circuits](../circuits/) | 1 |

## Body

Open `roadster.blend` in [Blender](https://www.blender.org/) and export STL files for printing. The views in `blueprints/` give the dimensions for fitting the body to the chassis.
