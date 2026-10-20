#pragma once
#include "ESP32AutoConnect.h"
#include <Arduino.h>
#include <WiFi.h>

namespace InitWiFi
{
  void    initESP32AutoConnect(const char hostname[]);
  uint8_t initWiFi(const char ssid[], const char password[], const char hostname[]);
  void    printConnectionDetails();
  void    printNearbyNetworks();
} // namespace InitWiFi