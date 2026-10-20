#pragma once
#include "Display.h"
#include <Arduino.h>
#include <LovyanGFX.hpp>
#include "RadioStorage.h"

namespace SDCard
{
  bool saveBmpToSD_16bit(LGFX& lcd, const char* filename);
  bool saveBmpToSD_24bit(LGFX& lcd, const char* filename);
} // namespace SDCard
