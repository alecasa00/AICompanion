#pragma once

#include <Arduino.h>

#if __has_include("secrets.h")
  #include "secrets.h"
#else
  #define WIFI_SSID "YOUR_WIFI_SSID"
  #define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
  #define GEMINI_API_KEY "YOUR_GEMINI_API_KEY"
#endif

// Hardware placeholders intentionally left undefined until the real board is available.
#define AUDIO_MIC_BCLK_PIN -1
#define AUDIO_MIC_WS_PIN -1
#define AUDIO_MIC_DATA_IN_PIN -1
#define AUDIO_MIC_MCLK_PIN -1
#define AUDIO_SPK_BCLK_PIN -1
#define AUDIO_SPK_WS_PIN -1
#define AUDIO_SPK_DATA_OUT_PIN -1
#define AUDIO_SPK_MCLK_PIN -1
#define AUDIO_INPUT_SAMPLE_RATE 16000
#define AUDIO_OUTPUT_SAMPLE_RATE 24000
#define AUDIO_BITS_PER_SAMPLE I2S_BITS_PER_SAMPLE_16BIT
#define AUDIO_CHANNELS 1
#define GEMINI_MODEL "models/gemini-3.1-flash-live-preview"

namespace ai_companion {
constexpr uint32_t kSerialBaudRate = 115200;
constexpr uint32_t kWifiTimeoutMs = 30000;
constexpr uint32_t kGeminiTimeoutMs = 30000;
constexpr uint32_t kAudioChunkFrames = 128;
constexpr uint32_t kAudioStreamCheckMs = 250;
}  // namespace ai_companion
