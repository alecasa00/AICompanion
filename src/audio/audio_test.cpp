#include "audio/audio_test.h"

namespace ai_companion {

void AudioTest::generateTone(int16_t* buffer, size_t sampleCount, uint32_t sampleRate, float frequencyHz) {
  if (buffer == nullptr || sampleCount == 0 || sampleRate == 0 || frequencyHz <= 0.0f) {
    return;
  }

  const float twoPi = 6.28318530718f;
  const float step = twoPi * frequencyHz / static_cast<float>(sampleRate);

  for (size_t i = 0; i < sampleCount; ++i) {
    const float value = sinf(step * static_cast<float>(i));
    buffer[i] = static_cast<int16_t>(value * 32767.0f * 0.25f);
  }
}

}  // namespace ai_companion
