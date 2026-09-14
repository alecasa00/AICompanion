#include <Arduino.h>

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

static SystemState gState = SystemState::BOOT;
static ai_companion::WifiManager gWifiManager;
static ai_companion::AudioInput gAudioInput;
static ai_companion::AudioOutput gAudioOutput;
static ai_companion::GeminiClient gGeminiClient;
static ai_companion::DisplayManager gDisplayManager;
static bool gGeminiStartRequested = false;
static int16_t gMicBuffer[128];
static int16_t gToneBuffer[128];
static unsigned long gLastAudioCheckMs = 0;
static bool gTonePlaybackArmed = false;

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

static void setState(SystemState nextState) {
  gState = nextState;
  gDisplayManager.showState(stateName(gState));

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
}

void setup() {
  Serial.begin(ai_companion::kSerialBaudRate);
  delay(200);
  Serial.println("\n============================");
  Serial.println("AICompanion boot sequence");
  Serial.println("============================");
  setState(SystemState::BOOT);
  gDisplayManager.begin();
  gDisplayManager.showState(stateName(gState));

  gWifiManager.begin();
  if (gAudioInput.begin()) {
    Serial.println("[AUDIO] Input ready");
  }
  if (gAudioOutput.begin()) {
    Serial.println("[AUDIO] Output ready");
    ai_companion::AudioTest::generateTone(gToneBuffer, sizeof(gToneBuffer) / sizeof(gToneBuffer[0]),
                                          AUDIO_OUTPUT_SAMPLE_RATE, 440.0f);
    gTonePlaybackArmed = true;
  }
  setState(SystemState::CONNECTING_WIFI);
}

void loop() {
  gWifiManager.update();
  gGeminiClient.update();

  const unsigned long now = millis();
  if (gAudioInput.isInitialized() && now - gLastAudioCheckMs >= ai_companion::kAudioStreamCheckMs) {
    gLastAudioCheckMs = now;
    const size_t samplesRead = gAudioInput.read(gMicBuffer, sizeof(gMicBuffer) / sizeof(gMicBuffer[0]));
    if (samplesRead > 0) {
      Serial.printf("[AUDIO] Captured %zu samples from microphone\n", samplesRead);
      gGeminiClient.sendAudio(reinterpret_cast<const uint8_t*>(gMicBuffer), samplesRead * sizeof(int16_t));
    }
  }

  if (gTonePlaybackArmed && gAudioOutput.isInitialized()) {
    gAudioOutput.write(gToneBuffer, sizeof(gToneBuffer) / sizeof(gToneBuffer[0]));
  }

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
      break;
    case SystemState::SPEAKING:
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

  delay(1000);
}
