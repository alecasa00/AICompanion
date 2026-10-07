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

// I moduli sono globali perché vengono inizializzati una volta e aggiornati nel loop Arduino.
static SystemState gState = SystemState::BOOT;
static ai_companion::WifiManager gWifiManager;
static ai_companion::AudioInput gAudioInput;
static ai_companion::AudioOutput gAudioOutput;
static ai_companion::GeminiClient gGeminiClient;
static ai_companion::DisplayManager gDisplayManager;
static bool gGeminiStartRequested = false;
static int16_t gMicBuffer[128];
static int16_t gNotificationBuffer[AUDIO_OUTPUT_SAMPLE_RATE / 20];
static unsigned long gLastAudioCheckMs = 0;

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

  if (stateChanged) {
    playStateNotification();
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
  // Legge periodicamente blocchi PCM dal microfono e li inoltra alla sessione Gemini.
  if (gAudioInput.isInitialized() && now - gLastAudioCheckMs >= ai_companion::kAudioStreamCheckMs) {
    gLastAudioCheckMs = now;
    const size_t samplesRead = gAudioInput.read(gMicBuffer, sizeof(gMicBuffer) / sizeof(gMicBuffer[0]));
    if (samplesRead > 0) {
      Serial.printf("[AUDIO] Captured %zu samples from microphone\n", samplesRead);
      gGeminiClient.sendAudio(reinterpret_cast<const uint8_t*>(gMicBuffer), samplesRead * sizeof(int16_t));
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
