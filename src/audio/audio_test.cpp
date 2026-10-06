#include "audio/audio_test.h"

namespace ai_companion {

void AudioTest::generateTone(int16_t* buffer, size_t sampleCount, uint32_t sampleRate, float frequencyHz) {
  if (buffer == nullptr || sampleCount == 0 || sampleRate == 0 || frequencyHz <= 0.0f) {
    return;
  }

  // Incremento di fase per campione: produce una sinusoide senza stato tra le chiamate.
  const float twoPi = 6.28318530718f;
  const float step = twoPi * frequencyHz / static_cast<float>(sampleRate);

  for (size_t i = 0; i < sampleCount; ++i) {
    const float value = sinf(step * static_cast<float>(i));
    // Riduce l'ampiezza al 25% del fondo scala per mantenere moderato il tono di prova.
    buffer[i] = static_cast<int16_t>(value * 32767.0f * 0.25f);
  }
}

}  // namespace ai_companion
