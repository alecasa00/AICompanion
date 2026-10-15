#pragma once

#include <Arduino.h>

#if __has_include("secrets.h")
  #include "secrets.h"
#elif __has_include("../../secrets.h")
  #include "../../secrets.h"
#endif

// Fallback per compilazioni senza il file locale o con campi non definiti.
#ifndef WIFI_SSID
  #define WIFI_SSID "YOUR_WIFI_SSID"
#endif
#ifndef WIFI_PASSWORD
  #define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#endif
#ifndef GEMINI_API_KEY
  #define GEMINI_API_KEY "YOUR_GEMINI_API_KEY"
#endif

// ============================================================
// AUDIO - I2S
// ============================================================

// Clock RX dedicati: i due driver I2S sono master e lavorano a frequenze diverse.
#define AUDIO_MIC_BCLK_PIN 18
#define AUDIO_MIC_WS_PIN   19

// Dato digitale in ingresso dal microfono INMP441.
#define AUDIO_MIC_DATA_IN_PIN 4

// L'INMP441 non usa MCLK.
#define AUDIO_MIC_MCLK_PIN -1


// Linee dati e clock dirette all'amplificatore MAX98357.
#define AUDIO_SPK_BCLK_PIN    16
#define AUDIO_SPK_WS_PIN      17
#define AUDIO_SPK_DATA_OUT_PIN 15

// Il MAX98357 non usa MCLK.
#define AUDIO_SPK_MCLK_PIN -1


// Formato dei flussi audio PCM per I2S.
#define AUDIO_INPUT_SAMPLE_RATE 16000
#define AUDIO_OUTPUT_SAMPLE_RATE 24000

#define AUDIO_INPUT_BITS_PER_SAMPLE I2S_BITS_PER_SAMPLE_32BIT
#define AUDIO_OUTPUT_BITS_PER_SAMPLE I2S_BITS_PER_SAMPLE_16BIT

#define AUDIO_CHANNELS 1


// ============================================================
// GEMINI
// ============================================================

#define GEMINI_MODEL "models/gemini-3.8-live"


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
#define BOARD_RGB_LED_PIN_V1_0 48
#define BOARD_RGB_LED_PIN_V1_1 38
#define BOARD_RGB_LED_BRIGHTNESS 12
// ============================================================
// APPLICATION
// ============================================================

namespace ai_companion {
// Timeout e intervalli condivisi dai moduli di rete e audio.
constexpr uint32_t kSerialBaudRate = 115200;
constexpr uint32_t kWifiTimeoutMs = 30000;
constexpr uint32_t kGeminiTimeoutMs = 30000;
// 512 campioni a 16 kHz = 32 ms per frame audio inviato a Gemini.
constexpr uint32_t kAudioChunkFrames = 512;
constexpr uint32_t kAudioStreamCheckMs = 250;
constexpr uint32_t kAudioIoTimeoutMs = 100;
}  // namespace ai_companion
