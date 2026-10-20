# Freenove FNK0104B (ESP32-S3, 2.8-inch ILI9341)

The `freenove-fnk0104b` PlatformIO environment targets the **factory-wired**
FNK0104B with 16 MB flash, 8 MB OPI PSRAM, an ILI9341 display and a
FT6236/FT6336-compatible capacitive touch controller. The original
`esp32-2432S028R` environment remains available.

## Verify the wiring before flashing

Freenove's FNK0104AB TFT configuration and touch/audio examples specify:

| Function | GPIO |
| --- | --- |
| LCD MOSI / MISO / SCLK / CS | 11 / 13 / 12 / 10 |
| LCD D/C / backlight | 46 / 45 |
| LCD reset | Board reset (no separate GPIO) |
| Touch and codec I2C SDA / SCL | 16 / 15 |
| Touch reset / interrupt | 18 / 17 |
| Touch I2C address | `0x38` |
| ES8311 I2C address | `0x18` |
| Audio MCLK / BCLK / WS / data out | 4 / 5 / 7 / 8 |
| Amplifier enable | 1 (active low) |
| SDMMC CLK / CMD / D0 (1-bit mode) | 38 / 40 / 39 |

**MOSI=13, MISO=12, SCLK=14, CS=15 is not the factory FNK0104B pinout.**
In particular, LCD CS=15 would conflict with the factory touch/codec I2C clock.
Do not select those SPI pins on an unmodified FNK0104B. For a physically rewired
board, confirm LCD D/C, reset, backlight, touch I2C/reset/interrupt and audio wiring
before changing the `FN_*` settings in `include/FreenovePins.h` (or overriding
supported settings with build flags). A compile-time check rejects pin conflicts.
Changing a GPIO in software does not move an onboard connection.

References:

- [Freenove ESP32-S3 Display repository](https://github.com/Freenove/Freenove_ESP32_S3_Display)
- [FNK0104AB display configuration archive](https://github.com/Freenove/Freenove_ESP32_S3_Display/blob/main/Libraries/FNK0104AB/TFT_eSPI_Setups_v1.3.zip)
- [Touch example](https://github.com/Freenove/Freenove_ESP32_S3_Display/tree/main/Tutorial_With_Touch/Sketches/Sketch_11.1_Touch)
- [Audio example](https://github.com/Freenove/Freenove_ESP32_S3_Display/tree/main/Tutorial_With_Touch/Sketches/Sketch_07.1_Music)
- [Schematic](https://github.com/Freenove/Freenove_ESP32_S3_Display/blob/main/Schematic/2.8inch_ESP32-S3_Display_Schematic.pdf)

## Build and upload

Use [pioarduino Core](https://github.com/pioarduino/platform-espressif32) 6.1.19
(the PlatformIO-compatible core), or the pioarduino IDE extension. The project pins the pioarduino
ESP32 platform with Arduino 3.3.7 and AudioTools 1.2.2.

```sh
pio run -e freenove-fnk0104b
pio run -e freenove-fnk0104b -t upload
pio device monitor -b 115200
```

Select the appropriate USB/serial device if multiple boards are connected. USB
CDC is enabled. No external I2S DAC is needed for the Freenove onboard speaker
output: the firmware initializes its ES8311 and active-low amplifier.

The UI is landscape (320x240). LovyanGFX handles the capacitive touch rotation;
the old resistive-touch mirroring/calibration is used only by the original CYD.
Touch and audio share I2C, which stays active when switching stations. SD uses a
separate SDMMC interface. Missing SD media does not prevent radio startup;
screenshots are saved under `/Screenshots` when a card is available.

Wi-Fi can be provisioned through the existing `AutoConnectAP` flow. Keep personal
credentials out of commits.

## Prepared cloud workspace

The cloud setup stores its tools and libraries under `/workspace` and supplies a
local configuration to avoid the restricted PlatformIO registry:

```sh
export PLATFORMIO_CORE_DIR=/workspace/.platformio
export PATH=/workspace/.platformio/tools/toolchain-xtensa-esp-elf/bin:/workspace/.venvs/iradio/bin:$PATH
cd /workspace/iradio
pio run -c /workspace/.setup/iradio/cloud.ini -e freenove-fnk0104b
```

`bash /workspace/.setup/iradio/install.sh` refreshes dependencies and builds this
target. It keeps generated IDE data outside the checkout. On another machine,
use the normal project commands above and allow access to the required package
registries and GitHub releases.

## Validation

Host pin configuration tests (requires Python 3 and g++):

```sh
python3 -m unittest discover -s test -p 'test_*.py' -v
```

A successful firmware build checks compilation and linking only. On the actual
board, verify display orientation and colors, touch at all corners and the volume
slider, Wi-Fi provisioning, audible playback and station switching, preference
recall after reboot, and screenshot creation with and without SD media. The
capacitive touch panel should remain responsive after a screenshot and a station
change. These behaviors cannot be confirmed without connected hardware.
