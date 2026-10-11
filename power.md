# Van Gogh — Power

Power distribution and budget for one barrel. Hub: [README.md](README.md).

---

## Barrel inlet

Van Gogh takes raw **6S** pack power (about 19–25.2 V, **22.2 V** nominal) through an **XT90 male** embedded in the power housing.

**6S in:** The feed to the barrel XT90 should be **10 AWG** ([BNTECHGO](https://www.amazon.com/BNTECHGO-Silicone-Flexible-Strands-Stranded/dp/B01ABOPO2S/)). That gauge stops at the XT90. Inside the power housing, stock **18 AWG** ESC battery leads and kit **22 AWG** Molex leads star at the cups. Harness gauges are in [Harness gauges](#harness-gauges) below.

Do not route flywheel current through the logic board.

---

## Split at the barrel base

Both XT90 leads split in **parallel** at the power housing:

| Branch | Path | Voltage | Wire / protection |
|--------|------|---------|-------------------|
| **Flywheel** | Thick pair to the two ESCs and two brushless motors | Unregulated 6S | 18 AWG; sized for tens of amps |
| **Housing** | Thinner pair to the barrel middle via the Molex, then the logic board | Same 6S, then local bucks | 22 AWG; **no fuse** |

On-board bucks on the barrel-middle logic board make:

- **+5 V** — MP1584EN mini buck module — ESP32-S3 Mini, laser, WS2812B.
- **+6 V** — [Pololu D36V50F6](https://www.pololu.com/product/4092) — both N20s via **one** DRV8833, plus solenoid.

---

## Housing regulators

| Rail | Module | Output | Realistic current |
|------|--------|--------|-------------------|
| Logic / accessory | MP1584EN mini buck, set to **5 V** | 5 V | About 1.5–2 A continuous, 3 A advertised peak. Housing +5 V peak is about 1.35 A. |
| Actuator | [Pololu D36V50F6](https://www.pololu.com/product/4092) | Fixed **6 V** | Typical **5.5 A** at 36 V in; about 3.3–8 A depending on input and cooling. Housing +6 V worst case is about 4.5 A (both N20s stall + solenoid). |

D36V50F6 extras: reverse-voltage protection, short-circuit protection, thermal shutdown, soft-start, ENABLE, power-good. Size is 25.4 × 25.4 mm. Socket it on headers or chassis-mount it in the metal housing. Use paired VIN/GND/VOUT pins or soldered wire; a single 0.1 in header pin is only 3 A.

Keep the MP1584 output set and measured at 5 V before first power-up. The D36V50F6 is fixed; do not add a feedback resistor to change it.

---

## Selected connector (base to middle)

**Kit:** yoeruyo Micro-Fit 3.0 **1×6** ([Amazon](https://www.amazon.com/dp/B0CL9MG9PY)) — single-row male/female housings with **22 AWG** pre-crimped leads. Use the **1×6** pair as the barrel-base ↔ barrel-middle uplink.

**Join:** wire-to-wire. Female 1×6 in the power housing; male 1×6 from the barrel middle. Short kit tails out the back of that male **solder to the logic board** J1 through-hole row (not a PCB-mount 43650). The Molex is the module plug; the board is a fixed install. Pad assignment: [board.md](board.md#j1).

**Pinout (2 / 2 / 2):**

| Pin | Function |
|-----|----------|
| 1 | 6S+ |
| 2 | 6S+ (paralleled with 1) |
| 3 | GND |
| 4 | GND (paralleled with 3) |
| 5 | ESC 1 signal |
| 6 | ESC 2 signal |

No regulated 5 V or 6 V crosses this joint — the bucks live on the logic board. Flywheel current stays on the XT90.

- ESC white/signal only on 5 and 6. Splice the thin ESC `S` lead to a kit 22 AWG into those pins — 22 AWG is more than enough for DShot (milliamps). No ESC BEC reds.
- Pins 1–4 feed both buck VIN/GND (MP1584EN +5 V and D36V50F6 +6 V).

---

## XT90 split in the power housing

Star both branches at the **XT90 male cups**. Do not daisy-chain the Molex off an ESC lead. Full gauge map: [Harness gauges](#harness-gauges).

**Positive cup**

1. ESC 1 battery + (stock **18 AWG**)
2. ESC 2 battery + (stock **18 AWG**)
3. Two kit **22 AWG** into Molex pins 1 and 2 (6S+)

**Negative cup**

1. ESC 1 battery − (stock **18 AWG**)
2. ESC 2 battery − (stock **18 AWG**)
3. Two kit **22 AWG** into Molex pins 3 and 4 (GND)

**Signals (not on the cups)**

- ESC 1 `S` spliced to a kit 22 AWG → Molex pin 5
- ESC 2 `S` spliced to a kit 22 AWG → Molex pin 6
- White/signal only. No ESC BEC reds.

Flywheel current stays on the 18 AWG. Housing current stays on the 22 AWG. Spinning the flywheels does **not** push motor current through the Molex — that path only draws what the bucks need (~0.2–2.1 A). The middle *does* see pack sag and ESC bus noise on the shared cups; both bucks are meant to run from raw 6S that moves around. Keep the two signal leads away from the motor **phase** wires inside the power housing.

No fuse on the housing tap. Do not route flywheel current through the Molex.

---

## Harness gauges

10 AWG stops at the power-housing XT90. Do **not** land 10 AWG in the Molex. Do **not** replace ESC battery leads with 22 AWG.

| Run | Gauge | Notes |
| --- | --- | --- |
| External feed → barrel XT90 | **10 AWG** | [BNTECHGO](https://www.amazon.com/BNTECHGO-Silicone-Flexible-Strands-Stranded/dp/B01ABOPO2S/). Planning ~22–32 A peak at this connector. Stops at the XT90 cups. |
| XT90 male → ESC 1 battery +/− | **18 AWG** | Stock LANRC 45A battery leads. ~10–15 A planning per motor. |
| XT90 male → ESC 2 battery +/− | **18 AWG** | Parallel at the cups with ESC 1, not daisy-chained. |
| XT90 male → Molex pins 1–2 (6S+) and 3–4 (GND) | **22 AWG** | [Micro-Fit kit](https://www.amazon.com/dp/B0CL9MG9PY). Housing tap only (~0.2–2.1 A). |
| Molex pins 5–6 → logic board J1 → ESP (DShot) | **22 AWG** | Signal only; 100 Ω series at ESP (R1, R2). |
| J2–J6 off-board loads | **24 AWG** | [EGSCST PH pigtails](https://www.amazon.com/dp/B0H3WF134X) to [JTSINERU PH headers](https://www.amazon.com/dp/B0F2M3LFNC). DIAL N20 run may need a splice. |

---

## Power rails and whether the connector is enough

The uplink carries **raw 6S only**. The two regulated rails are made **after** the connector, on the barrel-middle logic board.

| Rail | Where it exists | Source | Loads | Planning current |
|------|-----------------|--------|-------|------------------|
| Unregulated 6S | XT90 and ESC path; also the Molex tap | Pack, ~19–25.2 V (22.2 V nominal) | Two ESCs + two 2207s | ~20–30 A peak — **stays on XT90 / 18 AWG, not the Molex** |
| Unregulated 6S tap | Molex pins 1–4 | Same XT90 cups, 22 AWG | Both buck VIN/GND | **~0.2–0.4 A typical, ~1.8–2.1 A peak** from 6S |
| +5 V | Barrel middle only | MP1584EN | ESP32-S3 Mini, laser, WS2812B | Peak ~1.35 A at 5 V (~6.8 W) |
| +6 V | Barrel middle only | D36V50F6 | Both N20s via one DRV8833, solenoid | Worst ~4.5 A at 6 V (~27 W) if both stall + solenoid overlap |

Housing 6S input check (both bucks at peak, ~85% efficiency):

- Output: ~6.8 W + ~27 W ≈ **34 W**
- Pack side: 34 W / 22.2 V ≈ **1.5 A**; at a sagged 19.2 V ≈ **2.1 A**

Connector / wire vs that load:

- Two paralleled 22 AWG + two paralleled GND
- Micro-Fit-class pin ~5 A each; 22 AWG ~5 A
- Need ~2.1 A worst case → **about 2× margin on one pin, more with two paralleled**
- Voltage: 25.2 V into a 250 V+ connector family. Fine.
- Drop: 160 mm of 22 AWG at 1.5 A is tens of millivolts. The bucks still see ~22 V.

**Conclusion:** the 1×6 Micro-Fit and its 22 AWG leads transfer enough voltage and current for the barrel middle. They are not, and must not be used as, the flywheel feed.

---

## Power budget

Currents are planning estimates. Measure on the real hardware before locking copper.

| Component | Location | Voltage | Typical | Peak (can pull) |
|-----------|----------|---------|---------|-----------------|
| iFlight XING-E Pro 2207 1800KV (motor 1) | Flywheel carriage | 6S (~22.2 V) | TBD until flywheel load is measured | 10–15 A |
| iFlight XING-E Pro 2207 1800KV (motor 2) | Flywheel carriage | 6S (~22.2 V) | TBD until flywheel load is measured | 10–15 A |
| HNYYZL/LANRC 45A BLHeli_S ESC (×2) | Power housing | 6S (~22.2 V) | Follow paired motor | Follow paired motor (45 A rated) |
| Aideepen ESP32-S3 Mini | Barrel middle | +5 V | 100–250 mA | 500 mA (Wi-Fi TX) |
| HiLetgo 5 V 650 nm laser | Barrel middle | +5 V | 20–40 mA | 100 mA |
| WS2812B LEDs (12 px planning) | Barrel middle | +5 V | 50–200 mA (pattern) | 720 mA (full white) |
| DRV8833 (one module, both N20s, quiescent) | Barrel middle | +6 V | 5–20 mA | 50 mA |
| DWEII N20 DIAL indexer | Flywheel carriage | +6 V via U4 ch A | 100–300 mA running | 0.8–1.5 A stall |
| DWEII N20 cone tip | Barrel middle | +6 V via U4 ch B | 100–300 mA running | 0.8–1.5 A stall |
| MEETOOT SM0537 solenoid (6 V / 1.5 A) | Barrel middle | +6 V | 0 (off) | 1.5 A pulse (≤ 8 s) |

LED count is still 8–16 px. The 12 px row is the planning number. Solenoid current is pulse-only, not continuous.

| Total | Voltage | Typical | Peak |
|-------|---------|---------|------|
| Housing +5 V rail | 5 V | about 0.3–0.5 A (1.5–2.5 W) | about 1.35 A (6.8 W) |
| Housing +6 V rail | 6 V | about 0.3–0.7 A (1.8–4.2 W) | about 3.1–4.5 A (19–27 W) if both N20s stall and solenoid overlap |
| Housing input (after about 85% buck efficiency) | 6S (22.2 V nominal) | about 0.2–0.4 A (6 W) | about 1.8–2.1 A (34–40 W) |
| Flywheel carriage | 6S (22.2 V nominal) | TBD | about 20–30 A (450–750 W) |
| **Barrel XT90 (whole barrel)** | **6S (22.2 V nominal)** | **TBD + about 0.4 A housing** | **about 22–32 A** |

Size the housing tap and base-to-middle connector for **2–3 A continuous**, with short pulses above that. The XT90 and flywheel wiring are for the motor/ESC path, not the housing.

The D36V50F6 can supply the about 4.5 A both-stall-plus-solenoid stack. Avoiding that overlap in firmware is still good for motor and solenoid heat, but it is not required to protect the 6 V buck. **One** DRV8833 module drives both N20s — do not budget a second driver.
