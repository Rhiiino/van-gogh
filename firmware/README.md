# Van Gogh — Firmware

Component-test firmware for the **Aideepen ESP32-S3 Super Mini** socketed on the logic board (U3). The page runs each barrel load on its own: both flywheels, the DIAL indexer, the cone motor, the laser, the solenoid, and the LED stick.

Pin map and connectors: [board.md](../board.md). Power rails: [power.md](../power.md).

Open [index.html](index.html) in a browser to see the page without flashing. Buttons in that preview do not drive hardware. The board serves the same page after upload.

## Safety

This page can spin both flywheels, pulse the solenoid, and turn on the laser. Clamp the barrel or hold it, and keep a way to disconnect the 6S pack. The wheels store energy at speed and will grab anything that enters the nip. Do not look into the laser or aim it at anyone. Boot leaves every output off. **E-stop** parks them again.

## What the page drives

| Load | GPIO | Connector | Control |
| --- | --- | --- | --- |
| Flywheel 1 | 1 | Molex pin 5 | Arm, throttle, timed stop |
| Flywheel 2 | 2 | Molex pin 6 | Arm, throttle, timed stop |
| DIAL N20 | 4, 5 | J2 | Direction, timed run, coast or brake |
| Cone N20 | 6, 7 | J3 | Direction, timed run, coast or brake |
| Laser | 8 | J5 | Pulse, or stay on until you turn it off |
| Solenoid | 9 | J6 | Pulse only, 8 seconds maximum |
| WS2812 stick | 10 | J4 | Fill, one pixel, chase, off |

The two flywheels share one signal type (PWM or DShot) and one throttle cap. Each wheel still arms, runs, and stops on its own.

The indexer and cone motors are full 6 V while they run. A speed percentage would not change that, so the page does not offer one. Swap direction if a gearbox runs backward.

DRV8833 nSLEEP and nFAULT are not wired on this board. On the **back of the DRV8833 module**, solder-bridge **J2** or the driver stays asleep and neither N20 will turn. See [board.md](../board.md).

## Power

USB-C is for flashing, and it powers the ESP32 while the 5 V buck is off. The loads need their own rails:

| Rail | What it feeds |
| --- | --- |
| USB-C | Flash, and the ESP32 when the 5 V buck is off |
| +5 V buck (MP1584) | ESP32 5 V pin on pack power, laser, LED stick |
| +6 V buck (D36V50F6) | Indexer, cone motor, solenoid |
| 6S inlet | Flywheel speed controllers |

You can test the laser, the LEDs, the gearmotors, and the solenoid with the bucks on and the flywheel pack disconnected. Spinning a flywheel needs the 6S path, a clamped barrel, and a current limit you trust.

## Software

Use **Arduino IDE 2**.

1. **Arduino IDE > Settings** (on macOS: **Arduino IDE > Settings…**). Add this **Additional boards manager URL**:

```
https://espressif.github.io/arduino-esp32/package_esp32_index.json
```

2. **Tools > Board > Boards Manager…** — install **esp32** by Espressif Systems. This sketch was written for core **2.0.14**. The PWM and DShot paths also cover core 3.x.
3. **Sketch > Include Library > Manage Libraries…** — install:
   - **WiFiManager** by tzapu
   - **Adafruit NeoPixel** by Adafruit

## Board settings

Open `van_gogh/van_gogh.ino`. The folder name has to stay `van_gogh`.

| Tools menu | Value |
| --- | --- |
| Board | **ESP32S3 Dev Module** |
| USB CDC On Boot | **Enabled** |
| Upload Speed | **115200** |

Leave the other **ESP32S3 Dev Module** entries at their defaults. USB CDC has to be enabled or the board can stall on boot when nothing is reading the serial port, and the page never starts.

If the port never appears, or upload stops at “Connecting…”, hold **BOOT**, tap **RESET**, and release **BOOT** when the IDE says it is connecting.

## Wi-Fi

Two ways to get the board onto your network.

**Hotspot, no file to edit.** Leave `secrets.h` out. On first boot the board opens an access point named **`Van-Gogh-Setup`**. Join it from a phone, open `http://192.168.4.1`, pick your network, and save. The board then joins that network. Reconnect the phone to the same network.

**Saved network.** From `firmware/van_gogh/`:

```bash
cp secrets.h.example secrets.h
```

Put your SSID and password in `secrets.h`, then upload. `secrets.h` is gitignored. Empty strings behave like a missing file and fall back to the hotspot.

## Flash

1. Seat the Super Mini on U3. Plug in a data USB-C cable.
2. **Tools > Port** — the Super Mini’s port.
3. **Upload**.
4. **Tools > Serial Monitor** at **115200**. It prints the DHCP address. That address can change. The name `van-gogh.local` does not.

From a phone or computer on the same network, open **`http://van-gogh.local`**. If that name does not resolve, use the IP from the serial monitor. The header on the page shows the address it is using.

Some phones do not resolve `.local`. The IP from the serial line still works.

## How a test should go

1. **LED stick** — fill, then one pixel. A dark tail means data or solder stopped at the last lit pixel.
2. **Laser** — a short pulse. Confirm the beam, then turn it off.
3. **Solenoid** — a short pulse. You should hear or feel the click.
4. **DIAL**, then the **cone** — a short run, then the other direction if the shaft is backward.
5. **Flywheels** — apply **50 Hz PWM** and a **40%** cap, arm one wheel at idle, then a low throttle for a few seconds. Try the other wheel alone before either one sees a higher cap.

Changing the signal type disarms both flywheels. Arm again after **Apply signal and cap**. DShot repeats frames while that mode is selected. If a speed controller does not arm on DShot, stay on PWM.

## API

```bash
curl http://van-gogh.local/api/status
curl -X POST http://van-gogh.local/api/estop
curl -X POST http://van-gogh.local/api/flywheel/settings \
  -H 'Content-Type: application/json' \
  -d '{"protocol":"pwm","capPercent":40}'
curl -X POST http://van-gogh.local/api/flywheel/arm \
  -H 'Content-Type: application/json' -d '{"motor":1}'
curl -X POST http://van-gogh.local/api/flywheel/throttle \
  -H 'Content-Type: application/json' \
  -d '{"motor":1,"percent":20,"runMs":3000,"stop":"coast","capPercent":40}'
curl -X POST http://van-gogh.local/api/flywheel/stop \
  -H 'Content-Type: application/json' -d '{"motor":1,"stop":"cut"}'
curl -X POST http://van-gogh.local/api/dial/run \
  -H 'Content-Type: application/json' \
  -d '{"direction":"forward","runMs":1500,"stop":"coast"}'
curl -X POST http://van-gogh.local/api/cone/stop \
  -H 'Content-Type: application/json' -d '{"stop":"brake"}'
curl -X POST http://van-gogh.local/api/laser \
  -H 'Content-Type: application/json' -d '{"on":true,"pulseMs":500}'
curl -X POST http://van-gogh.local/api/solenoid \
  -H 'Content-Type: application/json' -d '{"pulseMs":300}'
curl -X POST http://van-gogh.local/api/leds \
  -H 'Content-Type: application/json' \
  -d '{"mode":"one","pixel":0,"count":12,"r":228,"g":177,"b":90,"brightness":40}'
```

`motor` is `1` or `2`. Laser `pulseMs` of `0` stays on. Solenoid pulses are clamped to 8000 ms.

## Files

| File | Role |
| --- | --- |
| [index.html](index.html) | The page. Open it locally to preview. |
| [van_gogh/van_gogh.ino](van_gogh/van_gogh.ino) | Sketch to upload. |
| [van_gogh/page.h](van_gogh/page.h) | The same page, stored in the sketch. |
| [van_gogh/secrets.h.example](van_gogh/secrets.h.example) | Wi-Fi template. Copy to `secrets.h`. |
| `van_gogh/pins.h`, `types.h`, `dshot.h`, `json_util.h` | Part of the sketch. Leave them in that folder. |
