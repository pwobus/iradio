#pragma once
#include <Arduino.h>
#include "RadioStorage.h"

namespace SDCard
{
  bool initSDCard();
  void endSDCard();
  void printSDCardInfo();
  void listFiles(File dir, int indent = 0);
} // namespace SDCard