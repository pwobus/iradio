#include "initSDCard.h"

namespace SDCard
{
#if !defined(IRADIO_FREENOVE_S3)
  static SPIClass sdcardSPI(VSPI);
#endif

  bool initSDCard()
  {
#if defined(IRADIO_FREENOVE_S3)
    // Factory FNK0104B slot: use 1-bit SDMMC (CLK, CMD, D0).
    if (!SD_MMC.setPins(38, 40, 39) || !SD_MMC.begin("/sdcard", true))
#else
    sdcardSPI.begin(TF_SCLK, TF_MISO, TF_MOSI, TF_CS);
    if (!SD.begin(TF_CS, sdcardSPI))
#endif
    {
      log_w("SD card unavailable; screenshots are disabled");
      endSDCard();
      return false;
    }
    return true;
  }

  void endSDCard()
  {
    card().end();
#if !defined(IRADIO_FREENOVE_S3)
    sdcardSPI.end();
#endif
  }

  /**
   * Use a raw string literal to print a formatted string of SD card details
   */
  void printSDCardInfo()
  {
    const char*   knownCardTypes[] = {"NONE", "MMC", "SDSC", "SDHC", "UNKNOWN"};
    sdcard_type_t cardType         = card().cardType();
    // uint64_t numSectors= SD.numSectors();
    // uint64_t sectorSize= SD.sectorSize();
    uint64_t cardSize              = card().cardSize() >> 20; // divide by 2^20 = 1'048'576 to get size in MB
    uint64_t cardTotal             = card().totalBytes() >> 20;
    uint64_t cardUsed              = card().usedBytes() >> 20;
    uint64_t cardFree              = cardTotal - cardUsed;
    Serial.printf(R"(
SDCard Info
-----------
type     %s
size   %6llu MB
total  %6llu MB
used   %6llu MB
free   %6llu MB
)",
                  knownCardTypes[cardType],
                  cardSize,
                  cardTotal,
                  cardUsed,
                  cardFree);
    Serial.printf("\n");
  }

  /**
   * Recursively lists all directories/files of
   * the file system starting at direcory dir
   *
   * Usage    listFiles(SD.open("/"));            // show content starting at root
   *          listFiles(SD.open("/SCREENSHOTS"))  // show content of directory SCREENSHOTS
   *
   * Code borrowed from:
   * https://wiki-content.arduino.cc/en/Tutorial/LibraryExamples/Listfiles *
   */
  void listFiles(File dir, int indent)
  {
    while (true)
    {
      File entry = dir.openNextFile();

      if (!entry)
        break; // no more files

      Serial.printf("%*c", indent * 4, ' ');

      if (entry.isDirectory())
      {
        Serial.printf("%s/\n", entry.name());
        listFiles(entry, indent + 1);
      }
      else
      {
        // files have sizes, directories do not
        Serial.printf("%s, %d\n", entry.name(), entry.size());
      }
      entry.close();
    }
  }
} // namespace SDCard
