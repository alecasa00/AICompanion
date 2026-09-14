#pragma once

#include <Arduino.h>

#if __has_include("secrets.h")
  #include "secrets.h"
#else
  #define WIFI_SSID "YOUR_WIFI_SSID"
  #define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
  #define GEMINI_API_KEY "YOUR_GEMINI_API_KEY"
#endif

// ============================================================
// AUDIO - I2S
// ============================================================

// Shared I2S clock lines
#define AUDIO_MIC_BCLK_PIN 16
#define AUDIO_MIC_WS_PIN   17

// INMP441 microphone data
#define AUDIO_MIC_DATA_IN_PIN 4

// INMP441 does not require MCLK
#define AUDIO_MIC_MCLK_PIN -1


// MAX98357 amplifier
#define AUDIO_SPK_BCLK_PIN    16
#define AUDIO_SPK_WS_PIN      17
#define AUDIO_SPK_DATA_OUT_PIN 15

// MAX98357 does not require MCLK
#define AUDIO_SPK_MCLK_PIN -1


// Audio format
#define AUDIO_INPUT_SAMPLE_RATE 16000
#define AUDIO_OUTPUT_SAMPLE_RATE 24000

#define AUDIO_BITS_PER_SAMPLE I2S_BITS_PER_SAMPLE_16BIT

#define AUDIO_CHANNELS 1


// ============================================================
// GEMINI
// ============================================================

#define GEMINI_MODEL "models/gemini-3.1-flash-live-preview"


// ============================================================
// OLED - I2C
// ============================================================

#define OLED_SDA_PIN 8
#define OLED_SCL_PIN 9

#define OLED_I2C_ADDRESS 0x3C

#define OLED_WIDTH 128
#define OLED_HEIGHT 64

// OLED reset not connected
#define OLED_RESET_PIN -1

// ============================================================
// BUTTON
// ============================================================
#define BUTTON_PIN 5
// ============================================================
// APPLICATION
// ============================================================

namespace ai_companion {
constexpr uint32_t kSerialBaudRate = 115200;
constexpr uint32_t kWifiTimeoutMs = 30000;
constexpr uint32_t kGeminiTimeoutMs = 30000;
constexpr uint32_t kAudioChunkFrames = 128;
constexpr uint32_t kAudioStreamCheckMs = 250;
}  // namespace ai_companion