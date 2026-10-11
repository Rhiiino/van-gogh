# Van Gogh — Logic board

Barrel control unit (BCU) PCB. Reference designators match the silkscreen. Editable KiCad project and fab zip: [kicad/](kicad/). Bill of materials: [bom.md](bom.md). Power rails and harness gauges: [power.md](power.md).

Socket **U1** (MP1584EN) and **U3** (ESP32-S3 Mini) with ProtoSupplies machined round headers — 0.50 mm tail on the PCB, 0.6 mm round pin on the module. Square 0.64 mm header pins do not fit those holes. **U2** and **U4** use standard 2.54 mm square female headers.

1206 resistors and ceramic capacitors are not polarized. Radial electrolytics: pad 1 is **+**, pad 2 is **−**. **Q1** and **Q2** are IRLR024N DPAK (1 G, 2 D, 3 S); the tab is drain (pin 2).

---

## J1

6-pin Molex inlet. **22 AWG** from the barrel-middle Micro-Fit male solder into these through-holes (not a PCB-mount 43650).

| Pin | Name | Connects to |
| --- | --- | --- |
| 1 | 6S+ | 6S+ (paralleled with pin 2) |
| 2 | 6S+ | 6S+ (paralleled with pin 1) |
| 3 | GND | GND (paralleled with pin 4) |
| 4 | GND | GND (paralleled with pin 3) |
| 5 | ESC1_DShot | R1 pin 2 → U3 GPIO1; ESC 1 signal from Molex |
| 6 | ESC2_DShot | R2 pin 2 → U3 GPIO2; ESC 2 signal from Molex |

---

## C1

100 µF 50 V electrolytic, 8 × 12 mm radial. Inlet bulk on 6S.

| Pin | Name | Connects to |
| --- | --- | --- |
| 1 | + | 6S+ |
| 2 | − | GND |

---

## U1

MP1584EN +5 V buck module (socketed).

| Pin | Name | Connects to |
| --- | --- | --- |
| 1 | VIN+ | 6S+ (both module holes) |
| 2 | VIN− | GND (both module holes) |
| 3 | VOUT− | GND (both module holes) |
| 4 | VOUT+ | 5V+ (both module holes) |

---

## C2

100 nF 50 V ceramic, 1206. HF decoupling at U1 VIN.

| Pin | Name | Connects to |
| --- | --- | --- |
| 1 | — | 6S+ |
| 2 | — | GND |

---

## U2

Pololu D36V50F6 +6 V buck (#4092), socketed. Three mounting holes are not electrical.

| Pin | Name | Connects to |
| --- | --- | --- |
| 1 | EN | Do not connect |
| 2 | PG | Do not connect |
| 3 | VRP | Do not connect |
| 4 | VRP | Do not connect |
| 5 | VIN | 6S+ |
| 6 | VIN | 6S+ |
| 7 | GND | GND |
| 8 | GND | GND |
| 9 | GND | GND |
| 10 | GND | GND |
| 11 | VOUT | 6V+ |
| 12 | VOUT | 6V+ (shared with pin 11) |

---

## C3

100 nF 50 V ceramic, 1206. HF decoupling at U2 VIN.

| Pin | Name | Connects to |
| --- | --- | --- |
| 1 | — | GND |
| 2 | — | 6S+ |

---

## C4

100 µF 16 V electrolytic radial. Bulk on +5 V.

| Pin | Name | Connects to |
| --- | --- | --- |
| 1 | + | 5V+ |
| 2 | − | GND |

---

## C5

10 µF 50 V ceramic, 1206. Mid-frequency on +5 V.

| Pin | Name | Connects to |
| --- | --- | --- |
| 1 | — | 5V+ |
| 2 | — | GND |

---

## C6

100 µF 16 V electrolytic radial. Bulk on +6 V.

| Pin | Name | Connects to |
| --- | --- | --- |
| 1 | + | 6V+ |
| 2 | − | GND |

---

## C7

10 µF 50 V ceramic, 1206. Mid-frequency on +6 V.

| Pin | Name | Connects to |
| --- | --- | --- |
| 1 | — | GND |
| 2 | — | 6V+ |

---

## U3

Aideepen ESP32-S3 Mini (socketed). Side-column pins only; module back pads are out of scope for this board.

| Pin | Label | Connects to |
| --- | --- | --- |
| 1 | TX | Unused (GPIO43) |
| 2 | RX | Unused (GPIO44) |
| 3 | GPIO1 | R1 pin 1 → J1 pin 5 (ESC1_DShot) |
| 4 | GPIO2 | R2 pin 1 → J1 pin 6 (ESC2_DShot) |
| 5 | GPIO3 | Unused (strapping — do not use) |
| 6 | GPIO4 | U4 pin 12 (INA1), DIAL N20 |
| 7 | GPIO5 | U4 pin 11 (INA2), DIAL N20 |
| 8 | GPIO6 | U4 pin 8 (INA3), cone N20 |
| 9 | GPIO7 | U4 pin 7 (INA4), cone N20 |
| 10 | 5V | 5V+ (from U1) |
| 11 | GND | GND |
| 12 | 3v3(out) | Unused on v1 — onboard LDO output; do not feed +5 V here |
| 13 | GPIO13 | Unused spare |
| 14 | GPIO12 | Unused spare |
| 15 | GPIO11 | Unused spare |
| 16 | GPIO10 | R3 pin 2 → J4 pin 2 (WS2812 data) |
| 17 | GPIO9 | R6 pin 2 → Q2 gate (solenoid) |
| 18 | GPIO8 | R4 pin 2 → Q1 gate (laser) |

---

## C8

100 nF 50 V ceramic, 1206. HF decoupling at U3 5 V.

| Pin | Name | Connects to |
| --- | --- | --- |
| 1 | — | 5V+ (U3 pin 10) |
| 2 | — | GND (U3 pin 11) |

---

## R1

100 Ω, 1206. DShot series damping, ESC 1.

| Pin | Name | Connects to |
| --- | --- | --- |
| 1 | — | U3 pin 3 (GPIO1) |
| 2 | — | J1 pin 5 (ESC1_DShot) |

---

## R2

100 Ω, 1206. DShot series damping, ESC 2.

| Pin | Name | Connects to |
| --- | --- | --- |
| 1 | — | U3 pin 4 (GPIO2) |
| 2 | — | J1 pin 6 (ESC2_DShot) |

---

## U4

WWZMDiB DRV8833 dual H-bridge module (socketed). One module drives **both** N20s. **VCC** is the +6 V motor rail, not 3.3 V.

On the **module back**, solder-bridge **J2** or the driver stays asleep (logic inputs work, outputs dead).

| Pin | Label | Connects to |
| --- | --- | --- |
| 1 | EEP (nSLEEP) | Unused — onboard 47 kΩ pull-up on module |
| 2 | OUT1 | J2 pin 1 (DIAL_M+) |
| 3 | OUT2 | J2 pin 2 (DIAL_M−) |
| 4 | OUT3 | J3 pin 1 (CONE_M+) |
| 5 | OUT4 | J3 pin 2 (CONE_M−) |
| 6 | ULT (nFAULT) | Unused |
| 7 | INA4 | U3 pin 9 (GPIO7) |
| 8 | INA3 | U3 pin 8 (GPIO6) |
| 9 | GND | GND |
| 10 | VCC | 6V+ |
| 11 | INA2 | U3 pin 7 (GPIO5) |
| 12 | INA1 | U3 pin 6 (GPIO4) |

---

## C9

100 nF 50 V ceramic, 1206. HF decoupling at U4 VCC.

| Pin | Name | Connects to |
| --- | --- | --- |
| 1 | — | GND (U4 pin 9) |
| 2 | — | 6V+ (U4 pin 10) |

---

## J2

2-pin right-angle JST-PH. DIAL N20 harness.

| Pin | Name | Connects to |
| --- | --- | --- |
| 1 | DIAL_M+ | U4 pin 2 (OUT1) |
| 2 | DIAL_M− | U4 pin 3 (OUT2) |

---

## J3

2-pin right-angle JST-PH. Cone N20 harness.

| Pin | Name | Connects to |
| --- | --- | --- |
| 1 | CONE_M+ | U4 pin 4 (OUT3) |
| 2 | CONE_M− | U4 pin 5 (OUT4) |

---

## R3

330 Ω, 1206. WS2812 data series.

| Pin | Name | Connects to |
| --- | --- | --- |
| 1 | — | J4 pin 2 (WS2812 data) |
| 2 | — | U3 pin 16 (GPIO10) |

---

## J4

3-pin right-angle JST-PH. WS2812B LED stick.

| Pin | Name | Connects to |
| --- | --- | --- |
| 1 | +5 V | 5V+ |
| 2 | Data | R3 pin 1 (WS2812 data) |
| 3 | GND | GND |

---

## R4

100 Ω, 1206. Laser MOSFET gate series.

| Pin | Name | Connects to |
| --- | --- | --- |
| 1 | — | Q1 pin 1 (G) |
| 2 | — | U3 pin 18 (GPIO8) |

---

## R5

10 kΩ, 1206. Laser MOSFET gate pull-down.

| Pin | Name | Connects to |
| --- | --- | --- |
| 1 | — | GND |
| 2 | — | Q1 pin 1 (G), with R4 |

---

## Q1

IRLR024N, DPAK. Low-side switch for laser.

| Pin | Name | Connects to |
| --- | --- | --- |
| 1 | G | R4 pin 1, R5 pin 2 |
| 2 | D | J5 pin 2 (tab = drain) |
| 3 | S | GND |

---

## J5

2-pin right-angle JST-PH. Laser module.

| Pin | Name | Connects to |
| --- | --- | --- |
| 1 | +5 V | 5V+ |
| 2 | Laser− | Q1 pin 2 (D) |

---

## R6

100 Ω, 1206. Solenoid MOSFET gate series.

| Pin | Name | Connects to |
| --- | --- | --- |
| 1 | — | Q2 pin 1 (G) |
| 2 | — | U3 pin 17 (GPIO9) |

---

## R7

10 kΩ, 1206. Solenoid MOSFET gate pull-down.

| Pin | Name | Connects to |
| --- | --- | --- |
| 1 | — | GND |
| 2 | — | Q2 pin 1 (G), with R6 |

---

## Q2

IRLR024N, DPAK. Low-side switch for solenoid.

| Pin | Name | Connects to |
| --- | --- | --- |
| 1 | G | R6 pin 1, R7 pin 2 |
| 2 | D | J6 pin 2, D1 pin 2 (A) |
| 3 | S | GND |

---

## D1

SS34 Schottky, SMA. Solenoid flyback. Band (cathode) on pad 1.

| Pin | Name | Connects to |
| --- | --- | --- |
| 1 | K (cathode) | 6V+ / J6 pin 1 |
| 2 | A (anode) | Q2 pin 2 (D) / J6 pin 2 |

Use SS34, SS36, or SS54. Do not use 1 A parts (SS12 / SS14).

---

## J6

2-pin right-angle JST-PH. Solenoid.

| Pin | Name | Connects to |
| --- | --- | --- |
| 1 | +6 V | 6V+ |
| 2 | Solenoid− | Q2 pin 2 (D) |
