#pragma once

#include <Arduino.h>

namespace ai_companion {

class AudioTest {
 public:
  static void generateTone(int16_t* buffer, size_t sampleCount, uint32_t sampleRate, float frequencyHz);
};

}  // namespace ai_companion
