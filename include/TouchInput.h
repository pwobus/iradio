#pragma once
#include <cstdint>

// Touch sampling and edge detection without sleeping in the audio-copy loop.
class TouchInput {
public:
  enum class Event { none, press, drag };

  bool sampleReady(uint32_t now) {
    if (static_cast<uint32_t>(now - lastSample) < 20) return false;
    lastSample = now;
    return true;
  }

  Event update(bool down, int x, int y) {
    if (!down) {
      wasDown = false;
      return Event::none;
    }
    const Event event = !wasDown ? Event::press
                       : (x != previousX || y != previousY) ? Event::drag
                                                            : Event::none;
    wasDown = true;
    previousX = x;
    previousY = y;
    return event;
  }

private:
  uint32_t lastSample = 0;
  bool wasDown = false;
  int previousX = 0, previousY = 0;
};
