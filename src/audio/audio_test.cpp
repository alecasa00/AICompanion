#include "audio/audio_test.h"

namespace ai_companion {

void AudioTest::generateTone(int16_t* buffer, size_t sampleCount, uint32_t sampleRate, float frequencyHz,
                             float amplitude) {
  if (buffer == nullptr || sampleCount == 0 || sampleRate == 0 || frequencyHz <= 0.0f || amplitude <= 0.0f) {
    return;
  }

  // Smussa l'attacco e il rilascio per evitare click ai bordi della notifica.
  const float twoPi = 6.28318530718f;
  const float step = twoPi * frequencyHz / static_cast<float>(sampleRate);
  const size_t fadeSamples = sampleRate / 200;

  for (size_t i = 0; i < sampleCount; ++i) {
    const float value = sinf(step * static_cast<float>(i));
    float envelope = 1.0f;
    if (fadeSamples > 0 && i < fadeSamples) {
      envelope = static_cast<float>(i) / static_cast<float>(fadeSamples);
    }
    const size_t samplesRemaining = sampleCount - i - 1;
    if (fadeSamples > 0 && samplesRemaining < fadeSamples) {
      const float release = static_cast<float>(samplesRemaining) / static_cast<float>(fadeSamples);
      if (release < envelope) {
        envelope = release;
      }
    }
    buffer[i] = static_cast<int16_t>(value * 32767.0f * amplitude * envelope);
  }
}

}  // namespace ai_companion
