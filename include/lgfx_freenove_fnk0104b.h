#pragma once
#include <LovyanGFX.hpp>
#include "FreenovePins.h"

// FT6236/FT6336 use the FT5x06-compatible I2C register interface.
class LGFX : public lgfx::LGFX_Device {
  lgfx::Panel_ILI9341 panel;
  lgfx::Bus_SPI bus;
  lgfx::Light_PWM light;
  lgfx::Touch_FT5x06 touchController;

public:
  LGFX() {
    auto spi = bus.config();
    spi.spi_host = SPI2_HOST;
    spi.spi_mode = 0;
    spi.freq_write = 40000000;
    spi.freq_read = 16000000;
    spi.spi_3wire = false;
    spi.use_lock = true;
    spi.dma_channel = SPI_DMA_CH_AUTO;
    spi.pin_mosi = FN_TFT_MOSI;
    spi.pin_miso = FN_TFT_MISO;
    spi.pin_sclk = FN_TFT_SCLK;
    spi.pin_dc = FN_TFT_DC;
    bus.config(spi);
    panel.setBus(&bus);

    auto lcd = panel.config();
    lcd.pin_cs = FN_TFT_CS;
    lcd.pin_rst = -1;
    lcd.pin_busy = -1;
    lcd.memory_width = lcd.panel_width = 240;
    lcd.memory_height = lcd.panel_height = 320;
    lcd.offset_rotation = 0;
    lcd.readable = true;
    lcd.invert = true;
    lcd.rgb_order = false; // BGR, as specified by Freenove's TFT setup.
    lcd.bus_shared = false; // SD uses SDMMC, touch uses I2C.
    panel.config(lcd);

    auto bl = light.config();
    bl.pin_bl = FN_TFT_BL;
    bl.invert = false;
    bl.freq = 20000;
    bl.pwm_channel = 0;
    light.config(bl);
    panel.setLight(&light);

    auto tp = touchController.config();
    tp.x_min = 0;
    tp.x_max = 239;
    tp.y_min = 0;
    tp.y_max = 319;
    tp.pin_int = FN_TOUCH_INT;
    tp.pin_rst = FN_TOUCH_RST;
    tp.i2c_port = 0;
    tp.i2c_addr = 0x38;
    tp.pin_sda = FN_TOUCH_SDA;
    tp.pin_scl = FN_TOUCH_SCL;
    tp.freq = 400000;
    tp.bus_shared = false; // Touch I2C is independent of the display SPI bus.
    tp.offset_rotation = 0;
    touchController.config(tp);
    panel.setTouch(&touchController);
    setPanel(&panel);
  }
};
