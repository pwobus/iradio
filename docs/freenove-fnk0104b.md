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

## No response from the touchscreen

The host tests above validate pin assignments; they do **not** exercise a physical
controller, I2C communication, or touch coordinates. Passing them does not prove
the touchscreen works.

The FT6236/FT6336 driver now polls over I2C instead of gating reads on GPIO 17.
Some controller/firmware combinations do not hold INT low throughout a contact,
so IRQ-gated reads can miss touches. Orientation has not been changed by this fix.
For a board verified to need level-sensitive IRQ reads, set
`-D FN_TOUCH_USE_IRQ=1` in that environment's build flags.

Build and upload the standalone diagnostic from your checkout on the computer
connected to the board:

```sh
pio run -e freenove-touch-test -t upload
pio device monitor -e freenove-touch-test -b 115200
```

The diagnostic mutes the amplifier and runs without Wi-Fi, radio streaming or SD
access. Touch each marked corner and the center. A green dot should follow your
finger. Copy the startup lines and several `read=...` lines from the serial monitor.
Raw I2C and mapped LGFX readings are separate samples, so an occasional mismatch
at press/release is normal.

- `controller 0x38 status=0` confirms an I2C acknowledgement.
- Repeated `read=0` indicates a communication problem; verify the physical board,
  SDA=16, SCL=15 and reset=18. An address acknowledgement alone does not verify
  successful coordinate reads.
- `read=1 contacts=0` on every touch means the device responds but reports no
  contacts; report this output and the exact board marking/controller model.
- `contacts=1` or `2` with changing raw coordinates confirms detection. Raw X/Y
  are meaningful only while `contacts` is nonzero.
- If raw contacts appear but `mapped=0` persists during a held touch, send the
  output; the library's controller probe/read path needs further investigation.
- If `mapped=1` but the dot is misplaced, report raw and mapped coordinates for
  the marked corners. That provides evidence for a rotation/calibration change.

After testing, restore the radio with
`pio run -e freenove-fnk0104b -t upload`. If the diagnostic works but the radio
stops responding, report whether it fails before playback, during playback, or
after switching stations/taking a screenshot.

## Audio breaks up when touched

The original radio loop slept for 200 ms on every detected touch, and control
handling added a further 100 ms delay. MP3 decoding/network copying runs in that
same loop, so touching or holding the screen could starve the audio output.

Touch sampling now runs every 20 ms without a blocking debounce delay. Buttons
activate once per contact, the volume slider accepts movement, and holding a
finger still does not redraw controls repeatedly. Audio copying continues between
samples. The host tests also cover contact/drag/release behavior and timer rollover;
they still do not measure audio quality on physical hardware.

After updating, check playback while holding an empty part of the screen and
while dragging the volume slider. Changing stations reconnects the stream and
screenshots intentionally stop/restart playback; those actions can still interrupt
audio. If ordinary touches still cause dropouts, report whether they occur only
while moving the slider or also while holding an empty area, and include any
serial errors.
