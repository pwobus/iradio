#pragma once
#include <Arduino.h>
#if defined(IRADIO_FREENOVE_S3)
#include <SD_MMC.h>
#else
#include <SD.h>
#endif

namespace SDCard {
inline auto& card() {
#if defined(IRADIO_FREENOVE_S3)
  return SD_MMC;
#else
  return SD;
#endif
}
} // namespace SDCard
