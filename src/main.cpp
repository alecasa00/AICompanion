#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <math.h>
#include <limits.h>
#include <string.h>

#include "audio/audio_input.h"
#include "audio/audio_output.h"
#include "audio/audio_test.h"
#include "config/config.h"
#include "display/display_manager.h"
#include "gemini/gemini_client.h"
#include "wifi/wifi_manager.h"

enum class SystemState {
  BOOT,
  CONNECTING_WIFI,
  WIFI_CONNECTED,
  CONNECTING_GEMINI,
  READY,
  LISTENING,
  SPEAKING,
  ERROR,
  RECONNECTING
};

// I moduli sono globali perché vengono inizializzati una volta e aggiornati nel loop Arduino.
static SystemState gState = SystemState::BOOT;
static ai_companion::WifiManager gWifiManager;
static ai_companion::AudioInput gAudioInput;
static ai_companion::AudioOutput gAudioOutput;
static ai_companion::GeminiClient gGeminiClient(gAudioOutput);
static ai_companion::DisplayManager gDisplayManager;
static bool gGeminiStartRequested = false;
static constexpr size_t kLocalPlaybackCapacitySamples = AUDIO_INPUT_SAMPLE_RATE * 5;
static int16_t gLocalPlaybackBuffer[kLocalPlaybackCapacitySamples];
static int16_t gMicBuffer[ai_companion::kAudioChunkFrames];
static int16_t gNotificationBuffer[AUDIO_OUTPUT_SAMPLE_RATE / 20];
static Adafruit_NeoPixel gBoardLedV10(1, BOARD_RGB_LED_PIN_V1_0, NEO_GRB + NEO_KHZ800);
static Adafruit_NeoPixel gBoardLedV11(1, BOARD_RGB_LED_PIN_V1_1, NEO_GRB + NEO_KHZ800);
static bool gPttButtonRawPressed = false;
static bool gPttButtonPressed = false;
static bool gPttRecording = false;
static bool gPttUnavailableReported = false;
static bool gLocalPlaybackPending = false;
static bool gLocalPlaybackActive = false;
static unsigned long gPttButtonChangedAtMs = 0;
static unsigned long gPttStartedAtMs = 0;
static unsigned long gLocalPlaybackAtMs = 0;
static size_t gPttSamplesSent = 0;
static size_t gPttSamplesCaptured = 0;
static size_t gPttSamplesStored = 0;
static size_t gPttNearFullScaleSamples = 0;
static size_t gLocalPlaybackSampleCount = 0;
static size_t gLocalPlaybackOffset = 0;
static size_t gLocalPlaybackOutputSampleCount = 0;
static size_t gPttChunksSent = 0;
static size_t gPttSendFailures = 0;
static int16_t gPttMinSample = INT16_MAX;
static int16_t gPttMaxSample = INT16_MIN;
static uint64_t gPttSumSquares = 0;

static void bootMarker(uint8_t marker) {
  Serial.printf("[BOOT-%02u]\n", marker);
}

// Converte gli stati interni in etichette brevi, usate sia sul display sia nei log.
static const char* stateName(SystemState state) {
  switch (state) {
    case SystemState::BOOT:
      return "BOOT";
    case SystemState::CONNECTING_WIFI:
      return "CONNECTING WIFI";
    case SystemState::WIFI_CONNECTED:
      return "WIFI CONNECTED";
    case SystemState::CONNECTING_GEMINI:
      return "CONNECTING GEMINI";
    case SystemState::READY:
      return "READY";
    case SystemState::LISTENING:
      return "LISTENING";
    case SystemState::SPEAKING:
      return "SPEAKING";
    case SystemState::ERROR:
      return "ERROR";
    case SystemState::RECONNECTING:
      return "RECONNECTING";
    default:
      return "UNKNOWN";
  }
}

static void logState(const char* label) {
  Serial.printf("[%s] %s\n", label, label);
}

// Emette una notifica breve e attenuata una sola volta per transizione.
static void playStateNotification() {
  if (!gAudioOutput.isInitialized()) {
    return;
  }

  ai_companion::AudioTest::generateTone(
      gNotificationBuffer, sizeof(gNotificationBuffer) / sizeof(gNotificationBuffer[0]),
      AUDIO_OUTPUT_SAMPLE_RATE, 880.0f, 0.06f);
  gAudioOutput.write(gNotificationBuffer,
                     sizeof(gNotificationBuffer) / sizeof(gNotificationBuffer[0]));
}

static void setMicrophoneLed(bool recording) {
  const uint32_t colorV10 = recording ? gBoardLedV10.Color(255, 0, 0) : 0;
  const uint32_t colorV11 = recording ? gBoardLedV11.Color(255, 0, 0) : 0;
  gBoardLedV10.setPixelColor(0, colorV10);
  gBoardLedV11.setPixelColor(0, colorV11);
  gBoardLedV10.show();
  gBoardLedV11.show();
}

static void updatePushToTalk(unsigned long now) {
  const bool rawPressed = (digitalRead(BUTTON_PIN) == LOW);
  if (rawPressed != gPttButtonRawPressed) {
    gPttButtonRawPressed = rawPressed;
    gPttButtonChangedAtMs = now;
  }

  if (now - gPttButtonChangedAtMs >= 30 && gPttButtonPressed != gPttButtonRawPressed) {
    gPttButtonPressed = gPttButtonRawPressed;
  }

  const bool shouldRecord = gPttButtonPressed && gAudioInput.isInitialized();
  if (gPttButtonPressed && !gAudioInput.isInitialized() && !gPttUnavailableReported) {
    Serial.println("[PTT] Microphone is not initialized");
    gPttUnavailableReported = true;
  } else if (!gPttButtonPressed) {
    gPttUnavailableReported = false;
  }
  if (shouldRecord == gPttRecording) {
    return;
  }

  gPttRecording = shouldRecord;
  setMicrophoneLed(gPttRecording);
  if (gPttRecording) {
    gPttStartedAtMs = now;
    gPttSamplesSent = 0;
    gPttSamplesCaptured = 0;
    gPttSamplesStored = 0;
    gPttNearFullScaleSamples = 0;
    gLocalPlaybackSampleCount = 0;
    gLocalPlaybackPending = false;
    gPttChunksSent = 0;
    gPttSendFailures = 0;
    gPttMinSample = INT16_MAX;
    gPttMaxSample = INT16_MIN;
    gPttSumSquares = 0;
    gDisplayManager.showState("RECORDING");
    Serial.println("[PTT] Recording started");
    if (!gGeminiClient.isSessionReady()) {
      Serial.println("[PTT] Gemini session unavailable; audio will not be sent yet");
    }
  } else {
    gDisplayManager.showState(stateName(gState));
    bool streamEnded = false;
    if (gGeminiClient.isSessionReady()) {
      streamEnded = gGeminiClient.endAudioStream();
    }
    const float rms = (gPttSamplesCaptured > 0)
                          ? sqrtf(static_cast<float>(gPttSumSquares) / gPttSamplesCaptured)
                          : 0.0f;
    Serial.printf("[PTT] Recording stopped: %lu ms, %u samples captured, %u sent, %u chunks sent, %u send failures, min=%d max=%d rms=%.1f, nearFullScale=%u (%.2f%%), audioStreamEnd=%s\n",
                  now - gPttStartedAtMs, static_cast<unsigned int>(gPttSamplesCaptured),
                  static_cast<unsigned int>(gPttSamplesSent),
                  static_cast<unsigned int>(gPttChunksSent),
                  static_cast<unsigned int>(gPttSendFailures), static_cast<int>(gPttMinSample),
                  static_cast<int>(gPttMaxSample), rms,
                  static_cast<unsigned int>(gPttNearFullScaleSamples),
                  gPttSamplesCaptured > 0
                      ? 100.0f * gPttNearFullScaleSamples / gPttSamplesCaptured
                      : 0.0f,
                  streamEnded ? "ok" : "failed");
    if (gPttSamplesCaptured > 0 && gAudioOutput.isInitialized()) {
      gLocalPlaybackSampleCount = gPttSamplesStored;
      if (gPttSamplesCaptured > gPttSamplesStored) {
        Serial.println("[LOOPBACK] Recording exceeded 5 seconds; replaying the first 5 seconds");
      }
      gLocalPlaybackOffset = 0;
      gLocalPlaybackOutputSampleCount =
          (gLocalPlaybackSampleCount * AUDIO_OUTPUT_SAMPLE_RATE + AUDIO_INPUT_SAMPLE_RATE - 1) /
          AUDIO_INPUT_SAMPLE_RATE;
      gLocalPlaybackAtMs = now + 2000;
      gLocalPlaybackPending = true;
      gLocalPlaybackActive = false;
      Serial.printf("[LOOPBACK] Replaying %u samples in 2 seconds\n",
                    static_cast<unsigned int>(gLocalPlaybackSampleCount));
    } else {
      Serial.println("[LOOPBACK] No recorded audio available for replay");
    }
  }
}

static void updateLocalPlayback(unsigned long now) {
  if (!gLocalPlaybackPending && !gLocalPlaybackActive) {
    return;
  }

  if (gLocalPlaybackPending && static_cast<int32_t>(now - gLocalPlaybackAtMs) >= 0) {
    gLocalPlaybackPending = false;
    gAudioOutput.clear();
    gLocalPlaybackOffset = 0;
    gLocalPlaybackActive = true;
    Serial.printf("[LOOPBACK] Playback started at %u Hz\n",
                  static_cast<unsigned int>(AUDIO_OUTPUT_SAMPLE_RATE));
  }

  if (!gLocalPlaybackActive) {
    return;
  }

  const size_t remaining = gLocalPlaybackOutputSampleCount - gLocalPlaybackOffset;
  const size_t chunkSamples = (remaining < ai_companion::kAudioChunkFrames)
                                  ? remaining
                                  : ai_companion::kAudioChunkFrames;
  int16_t outputBuffer[ai_companion::kAudioChunkFrames];
  for (size_t i = 0; i < chunkSamples; ++i) {
    const size_t outputIndex = gLocalPlaybackOffset + i;
    const uint64_t sourcePositionNumerator =
        static_cast<uint64_t>(outputIndex) * AUDIO_INPUT_SAMPLE_RATE;
    const size_t sourceIndex = sourcePositionNumerator / AUDIO_OUTPUT_SAMPLE_RATE;
    const uint32_t fraction = sourcePositionNumerator % AUDIO_OUTPUT_SAMPLE_RATE;
    const int32_t first = gLocalPlaybackBuffer[sourceIndex];
    const int32_t second = gLocalPlaybackBuffer[(sourceIndex + 1 < gLocalPlaybackSampleCount)
                                                    ? sourceIndex + 1
                                                    : sourceIndex];
    const int32_t interpolated =
        first + ((second - first) * static_cast<int32_t>(fraction)) /
                    static_cast<int32_t>(AUDIO_OUTPUT_SAMPLE_RATE);
    // I campioni acquisiti arrivano spesso vicino al full scale. Attenuiamo il
    // loopback locale per evitare clipping/distorsione e picchi di corrente.
    int32_t attenuated = interpolated / 8;
    constexpr size_t kFadeSamples = AUDIO_OUTPUT_SAMPLE_RATE / 100;  // 10 ms
    const size_t remainingAfterSample = gLocalPlaybackOutputSampleCount - outputIndex - 1;
    if (outputIndex < kFadeSamples) {
      attenuated = attenuated * static_cast<int32_t>(outputIndex + 1) /
                   static_cast<int32_t>(kFadeSamples);
    }
    if (remainingAfterSample < kFadeSamples) {
      attenuated = attenuated * static_cast<int32_t>(remainingAfterSample) /
                   static_cast<int32_t>(kFadeSamples);
    }
    outputBuffer[i] = static_cast<int16_t>(attenuated);
  }
  const size_t written = gAudioOutput.write(outputBuffer, chunkSamples);
  if (written == 0) {
    Serial.println("[LOOPBACK] Speaker write failed; stopping playback");
    gAudioOutput.clear();
    gLocalPlaybackActive = false;
    return;
  }
  gLocalPlaybackOffset += written;

  if (gLocalPlaybackOffset >= gLocalPlaybackOutputSampleCount) {
    delay(25);
    gLocalPlaybackActive = false;
    Serial.printf("[LOOPBACK] Playback complete: %u output samples\n",
                  static_cast<unsigned int>(gLocalPlaybackOutputSampleCount));
  }
}

static void setState(SystemState nextState) {
  const bool stateChanged = (nextState != gState);
  gState = nextState;
  gDisplayManager.showState(stateName(gState));

  // Ogni transizione ha un messaggio dedicato per facilitare il debug via seriale.
  switch (gState) {
    case SystemState::BOOT:
      Serial.println("[BOOT] AICompanion starting");
      break;
    case SystemState::CONNECTING_WIFI:
      Serial.println("[WIFI] Connecting...");
      break;
    case SystemState::WIFI_CONNECTED:
      Serial.println("[WIFI] Connected");
      break;
    case SystemState::CONNECTING_GEMINI:
      Serial.println("[WS] Connecting to Gemini...");
      break;
    case SystemState::READY:
      Serial.println("[AI] Session ready");
      break;
    case SystemState::LISTENING:
      Serial.println("[AI] Listening");
      break;
    case SystemState::SPEAKING:
      Serial.println("[AI] Response started");
      break;
    case SystemState::ERROR:
      Serial.println("[ERROR] Recoverable error state");
      break;
    case SystemState::RECONNECTING:
      Serial.println("[WS] Reconnecting...");
      break;
    default:
      Serial.println("[STATE] Unknown state");
      break;
  }

  if (stateChanged && gState != SystemState::LISTENING && gState != SystemState::SPEAKING) {
    playStateNotification();
  }
}

void setup() {
  Serial.begin(ai_companion::kSerialBaudRate);
  delay(200);
  Serial.println("\n============================");
  Serial.println("AICompanion boot sequence");
  Serial.println("============================");
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  gPttButtonRawPressed = (digitalRead(BUTTON_PIN) == LOW);
  gPttButtonPressed = gPttButtonRawPressed;
  gPttButtonChangedAtMs = millis();
  gBoardLedV10.begin();
  gBoardLedV11.begin();
  gBoardLedV10.setBrightness(BOARD_RGB_LED_BRIGHTNESS);
  gBoardLedV11.setBrightness(BOARD_RGB_LED_BRIGHTNESS);
  setMicrophoneLed(false);
  setState(SystemState::BOOT);
  gDisplayManager.begin();
  gDisplayManager.showState(stateName(gState));

  gWifiManager.begin();
  // I moduli audio possono restare inattivi se i pin richiesti non sono configurati.
  if (gAudioInput.begin()) {
    Serial.println("[AUDIO] Input ready");
  }
  if (gAudioOutput.begin()) {
    Serial.println("[AUDIO] Output ready");
  }
  setState(SystemState::CONNECTING_WIFI);
}

void loop() {
  // I client di rete vengono aggiornati frequentemente per gestire eventi asincroni.
  gWifiManager.update();
  gGeminiClient.update();

  const unsigned long now = millis();
  updatePushToTalk(now);
  updateLocalPlayback(now);

  // Il microfono viene letto e inviato soltanto mentre il pulsante PTT è premuto.
  if (gPttRecording) {
    const size_t samplesRead = gAudioInput.read(gMicBuffer, sizeof(gMicBuffer) / sizeof(gMicBuffer[0]));
    if (samplesRead > 0) {
      for (size_t i = 0; i < samplesRead; ++i) {
        const int16_t sample = gMicBuffer[i];
        if (sample < gPttMinSample) gPttMinSample = sample;
        if (sample > gPttMaxSample) gPttMaxSample = sample;
        if (sample <= -32760 || sample >= 32760) ++gPttNearFullScaleSamples;
        const int64_t signedSample = sample;
        gPttSumSquares += static_cast<uint64_t>(signedSample * signedSample);
      }
      const size_t room = kLocalPlaybackCapacitySamples - gPttSamplesStored;
      const size_t samplesToStore = (samplesRead < room) ? samplesRead : room;
      if (samplesToStore > 0) {
        memcpy(gLocalPlaybackBuffer + gPttSamplesStored, gMicBuffer,
               samplesToStore * sizeof(int16_t));
        gPttSamplesStored += samplesToStore;
      }
      gPttSamplesCaptured += samplesRead;

      if (gGeminiClient.isSessionReady()) {
        if (gGeminiClient.sendAudio(reinterpret_cast<const uint8_t*>(gMicBuffer), samplesRead * sizeof(int16_t))) {
          gPttSamplesSent += samplesRead;
          ++gPttChunksSent;
        } else {
          ++gPttSendFailures;
        }
      }
    }
  }

  // La macchina a stati avanza senza attendere le operazioni di rete.
  switch (gState) {
    case SystemState::BOOT:
      setState(SystemState::CONNECTING_WIFI);
      break;
    case SystemState::CONNECTING_WIFI:
      if (gWifiManager.isConnected()) {
        setState(SystemState::WIFI_CONNECTED);
      }
      break;
    case SystemState::WIFI_CONNECTED:
      if (!gGeminiStartRequested) {
        gGeminiClient.begin();
        gGeminiStartRequested = true;
        setState(SystemState::CONNECTING_GEMINI);
      }
      break;
    case SystemState::CONNECTING_GEMINI:
      if (gGeminiClient.isSessionReady()) {
        setState(SystemState::READY);
      }
      break;
    case SystemState::READY:
      setState(SystemState::LISTENING);
      break;
    case SystemState::LISTENING:
      if (gGeminiClient.isResponseActive()) {
        setState(SystemState::SPEAKING);
      }
      break;
    case SystemState::SPEAKING:
      if (!gGeminiClient.isResponseActive()) {
        setState(SystemState::LISTENING);
      }
      break;
    case SystemState::ERROR:
      setState(SystemState::RECONNECTING);
      break;
    case SystemState::RECONNECTING:
      gWifiManager.begin();
      setState(SystemState::CONNECTING_WIFI);
      break;
    default:
      setState(SystemState::BOOT);
      break;
  }

  delay(1);
}
