# Van Gogh — BOM

Everything required to build **one** barrel. Quantities are per barrel. Hub: [README.md](README.md).

Part numbers and board placement: [board.md](board.md). Bought-part cost: [cost.md](cost.md).

---

## Printed

One row is one printed part. Qty is per barrel. **Specifications** is material and slicer settings when recorded. Images: [catalog.md](catalog.md).


| Part                       | Qty | Specifications    |
| -------------------------- | --- | ----------------- |
| actuation rod              | 1   | 100% infill, PETG |
| ammo guide                 | 1   | 100% infill, PETG |
| barrel base L panel        | 1   | 100% infill, PETG |
| barrel base R panel        | 1   | 100% infill, PETG |
| barrel middle              | 1   | 100% infill, PETG |
| BCU cover                  | 1   |                   |
| cone                       | 1   | 100% infill, PETG |
| drum cam                   | 1   | 0.08mm quality, 100% infill, PETG |
| drum main                  | 1   | 0.08mm quality, 100% infill, PETG |
| drum motor shaft           | 1   | 100% infill, PETG |
| ESC casing                 | 2   | 100% infill, PETG |
| flywheel                   | 2   |                   |
| flywheel base              | 1   | 100% infill, PETG |
| flywheel carriage          | 1   | 100% infill, PETG |
| lasenoid base              | 1   |                   |
| laser carriage             | 1   |                   |
| led & cone n20 motor panel | 1   |                   |
| magazine lid               | 1   |                   |
| magazine main              | 1   |                   |
| magazine mount             | 1   |                   |
| magazine pusher            | 1   | 0.08mm quality, 100% infill, PETG |
| module - name plate        | 1   |                   |
| name & lasenoid panel      | 1   |                   |
| PH ceiling / PYE connector | 1   |                   |
| pinion gear                | 1   | 100% infill, PETG |
| plunger                    | 1   | 100% infill, PETG |
| plunger mount              | 1   | 100% infill, PETG |
| power housing              | 1   | 100% infill, PETG |
| solenoid / rod pin         | 1   |                   |


## Fasteners

A magnet join is one magnet on each part, opposite poles facing, so the two parts hold together.

### Magnets

One row is one join. **Qty each** is the magnet count on that part and on its counterpart.


| Join                                        | Part                       | Counterpart       | Qty each | Total |
| ------------------------------------------- | -------------------------- | ----------------- | -------- | ----- |
| BCU cover to barrel middle                  | BCU cover                  | Barrel middle     | 2        | 4     |
| Name & lasenoid panel to barrel middle      | name & lasenoid panel      | Barrel middle     | 2        | 4     |
| LED & cone N20 motor panel to barrel middle | led & cone n20 motor panel | Barrel middle     | 2        | 4     |
| Barrel base L panel to flywheel carriage    | barrel base L panel        | flywheel carriage | 1        | 2     |
| Barrel base R panel to flywheel carriage    | barrel base R panel        | flywheel carriage | 1        | 2     |


### M3 bolts and heat-set inserts

One row is one bolt path. Every bolt is an M3. **Through** is the part under the bolt head, then any parts the bolt passes, in that order. **Insert in** is the last part, the one with the heat-set insert. **Length** is the bolt length. **Qty** is the number of bolts on that path. Each bolt has one insert, except the flywheel-motor bolts, which thread into the motor.


| Join                                                 | Through                       | Insert in         | Length | Qty |
| ---------------------------------------------------- | ----------------------------- | ----------------- | ------ | --- |
| Magazine mount to barrel middle                      | magazine mount                | barrel middle     | 6 mm   | 2   |
| Magazine mount to flywheel carriage                  | magazine mount                | flywheel carriage | 6 mm   | 1   |
| Logic board to barrel middle                         | logic board                   | barrel middle     | 6 mm   | 1   |
| Plunger mount to flywheel base                       | plunger mount                 | flywheel base     | 35 mm  | 1   |
| Ammo guide to flywheel base                          | flywheel base                 | ammo guide        | 12 mm  | 1   |
| Flywheel carriage to power housing and flywheel base | power housing → flywheel base | flywheel carriage | 12 mm  | 2   |
| Power housing to flywheel base                       | power housing                 | flywheel base     | 6 mm   | 1   |
| Flywheel motor to flywheel base                      | flywheel base                 | —                 | 6 mm   | 4   |
| PH ceiling / PYE connector to power housing          | PH ceiling / PYE connector    | power housing     | 8 mm   | 3   |


The flywheel-motor path is four bolts, two per motor. Each bolt passes through the **flywheel base** and threads into the **Flywheel motor**. There is no heat-set insert on that path.

## Mechanical


| Part           | Qty | Spec                           | Listing |
| -------------- | --- | ------------------------------ | ------- |
| Ring bearing   | 1   | uxcell 6706-2RS, 30 × 37 × 4 mm | [Amazon](https://www.amazon.com/dp/B082PSDLTS) |
| Solenoid       | 1   | MEETOOT SM0537, 6 V / 1.5 A    | — |
| Flywheel motor | 2   | iFlight XING-E Pro 2207 1800KV | [Amazon](https://www.amazon.com/iFlight-1800KV-Brushless-Racing-Quadcopter/dp/B0FS12G8LT/) |
| LED stick      | 1   | WS2812B                        | — |
| ESC            | 2   | HNYYZL/LANRC 45A BLHeli_S      | [Amazon](https://www.amazon.com/HNYYZL-Brushless-Electronic-Controller-Quadcopter/dp/B0DXP957FD/) |
| N20 motor      | 2   | DWEII                          | — |
| Plunger spring | 1   | 0.5 × 6 × 30 mm (Glarks kit)   | [Amazon](https://www.amazon.com/dp/B09PY68K9C) |


The ring bearing spins the cone. The plunger spring is the 30 mm piece from the Glarks assortment.

## Connectors and wire


| Part                     | Qty | Where         | Listing |
| ------------------------ | --- | ------------- | ------- |
| Micro-Fit 3.0 1×6 female | 1   | Power housing | [Amazon](https://www.amazon.com/dp/B0CL9MG9PY) |
| Micro-Fit 3.0 1×6 male   | 1   | Barrel middle | same kit |
| XT90 male                | 1   | Power housing | — |


## Logic board

The PCB and the parts soldered on it. Qty is per barrel. Silkscreen refs and pins: [board.md](board.md). Bare board: order [kicad/gerber.zip](kicad/gerber.zip). Design source: [kicad/](kicad/).


| Part                             | Qty | Spec                                                            |
| -------------------------------- | --- | --------------------------------------------------------------- |
| Van Gogh logic PCB               | 1   | 56 × 92.6 mm, 2-layer, 1.6 mm, 1 oz. Fab: [gerber.zip](kicad/gerber.zip) |
| ESP32-S3 Mini                    | 1   | Aideepen. U3. [Amazon](https://www.amazon.com/dp/B0H3PJD525) |
| MP1584EN                         | 1   | +5 V buck. U1. [Amazon](https://www.amazon.com/dp/B01MQGMOKI) |
| Pololu D36V50F6                  | 1   | +6 V buck, #4092. U2. [Pololu](https://www.pololu.com/product/4092) |
| DRV8833 module                   | 1   | WWZMDiB. Both N20s. U4. [Amazon](https://www.amazon.com/dp/B0DB8CX8LK) |
| 100 µF 50 V electrolytic         | 1   | 8 × 12 mm. C1, 6S inlet. [Amazon](https://www.amazon.com/dp/B0CMQ8XQ4N) |
| 100 µF 16 V electrolytic         | 2   | C4 on +5 V, C6 on +6 V. [Amazon](https://www.amazon.com/dp/B07PBQXQNQ) |
| 100 nF 50 V ceramic              | 4   | 1206. C2, C3, C8, C9. [Amazon](https://www.amazon.com/dp/B0F5Q8V6BH) |
| 10 µF 50 V ceramic               | 2   | 1206. C5 on +5 V, C7 on +6 V. same kit |
| 100 Ω                            | 4   | 1206. R1, R2 DShot; R4, R6 gate series. [Amazon](https://www.amazon.com/dp/B08RYMYWG9) |
| 10 kΩ                            | 2   | 1206. R5, R7 gate pull-downs. same kit |
| 330 Ω                            | 1   | 1206. R3, LED data. same kit |
| IRLR024N                         | 2   | DPAK. Q1 laser, Q2 solenoid. [Amazon](https://www.amazon.com/dp/B08M414XR6) |
| SS34                             | 1   | SMA. D1 solenoid flyback. [Amazon](https://www.amazon.com/dp/B0F5W4TZRS) |
| JST-PH 2.0 mm right-angle, 2-pin | 4   | J2 DIAL N20, J3 cone N20, J5 laser, J6 solenoid. [Amazon](https://www.amazon.com/dp/B0F2M3LFNC) |
| JST-PH 2.0 mm right-angle, 3-pin | 1   | J4 LED stick. same listing |
| Machined round female header     | 6   | 2 × 1×9 on U3, 4 × 1×2 on U1. [ProtoSupplies 24306](https://protosupplies.com/product/header-machined-round-female-1x40/) |
| Machined round male header       | 6   | On ESP32-S3 Mini and MP1584. [ProtoSupplies](https://protosupplies.com/product/header-machined-round-pin-male-1x40/) |
| 2.54 mm square female header     | 2   | 1×6 U4 socket; U2 Pololu socket |


The Micro-Fit tails solder into the J1 pad row on the logic board. That connector is in **Connectors and wire**. The N20s, solenoid, LED stick, ESCs, and flywheel motors are in **Mechanical**.
