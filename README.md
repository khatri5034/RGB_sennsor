# RGB Color Sensor (TCS34725)

A C library and demo program that reads live color data from a **TCS34725 RGB color sensor** over I2C on a Raspberry Pi, converts the raw readings into a normalized RGB value, and matches it against a palette of known colors with a confidence score.

## How it works

1. `ColorLib_Init()` verifies the sensor over I2C (checks device ID `0x44`/`0x4D`), then configures integration time and gain and powers on the ADC.
2. `ColorLib_GetMatch()` reads the raw Clear/Red/Green/Blue channels, normalizes R/G/B against the Clear channel to cancel out ambient brightness, scales the brightest channel to full intensity, and applies gamma correction (γ = 2.2) to approximate how the color appears to the human eye.
3. The resulting RGB value is compared (Euclidean distance in RGB space) against a table of 16 predefined target colors (`PROJECT_COLORS` in `ColorLib.h`) to find the closest match and compute a confidence percentage.
4. `main.c` loops forever, printing the matched color's hex value and name every 500ms, and exits cleanly on `Ctrl+C` (SIGINT).

```
Hex: #FF0000 | Color: Red
Hex: #00FF00 | Color: Green
```

A demo video (`rgb-sensor-video.mp4`) and the assignment write-up (`Assignment5_GroupProject_RGBSensor.pdf`) are included in this repo.

## Hardware

- Raspberry Pi with I2C enabled
- TCS34725 RGB color sensor (I2C address `0x29`), wired to the Pi's I2C bus

## Project layout

| File | Purpose |
|---|---|
| `main.c` | Program entry point; polls the sensor and prints color matches |
| `lib/ColorLib.c` / `.h` | Sensor driver + color-matching library (designed to be reusable in other projects) |
| `lib/DEV_Config.c` / `.h` | Low-level I2C/GPIO hardware abstraction |
| `Makefile` | Build configuration (outputs to `dist/`) |

## Build & run

```bash
make            # compiles to ./assignment5
sudo ./assignment5
```

Run `make clean` to remove build artifacts.

## Team

Group project for CSC 615, San Francisco State University — Haibin Cao, Eric Ahsue, Kiran Khatri, John Tsiglieris
