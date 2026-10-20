#pragma once

// Factory FNK0104B wiring, from Freenove's FNK0104AB TFT setup and examples.
// Override these only for a physically rewired display.
#ifndef FN_TFT_MOSI
#define FN_TFT_MOSI 11
#endif
#ifndef FN_TFT_MISO
#define FN_TFT_MISO 13
#endif
#ifndef FN_TFT_SCLK
#define FN_TFT_SCLK 12
#endif
#ifndef FN_TFT_CS
#define FN_TFT_CS 10
#endif
#ifndef FN_TFT_DC
#define FN_TFT_DC 46
#endif
#ifndef FN_TFT_BL
#define FN_TFT_BL 45
#endif
#ifndef FN_TOUCH_SDA
#define FN_TOUCH_SDA 16
#endif
#ifndef FN_TOUCH_SCL
#define FN_TOUCH_SCL 15
#endif
#define FN_TOUCH_RST 18
#define FN_TOUCH_INT 17
#define FN_I2S_MCLK 4
#define FN_I2S_BCLK 5
#define FN_I2S_WS 7
#define FN_I2S_DOUT 8
#define FN_AUDIO_ENABLE 1

namespace freenove {
constexpr int pins[] = {FN_TFT_MOSI, FN_TFT_MISO, FN_TFT_SCLK, FN_TFT_CS,
                       FN_TFT_DC, FN_TFT_BL, FN_TOUCH_SDA, FN_TOUCH_SCL,
                       FN_TOUCH_RST, FN_TOUCH_INT, FN_I2S_MCLK, FN_I2S_BCLK,
                       FN_I2S_WS, FN_I2S_DOUT, FN_AUDIO_ENABLE, 38, 40, 39};
constexpr bool distinctPins() {
  for (unsigned i = 0; i < sizeof(pins) / sizeof(pins[0]); ++i)
    for (unsigned j = i + 1; j < sizeof(pins) / sizeof(pins[0]); ++j)
      if (pins[i] >= 0 && pins[i] == pins[j]) return false;
  return true;
}
static_assert(distinctPins(), "FNK0104B pin conflict: LCD, touch/audio I2C, I2S and SD must use distinct GPIOs");
}
