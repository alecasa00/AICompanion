#include "gemini/gemini_client.h"

#include <ArduinoJson.h>
#include <base64.h>
#include <stdlib.h>
#include <string.h>

#include "config/config.h"

namespace ai_companion {

namespace {

int base64Value(char character) {
  if (character >= 'A' && character <= 'Z') return character - 'A';
  if (character >= 'a' && character <= 'z') return character - 'a' + 26;
  if (character >= '0' && character <= '9') return character - '0' + 52;
  if (character == '+') return 62;
  if (character == '/') return 63;
  return -1;
}

bool decodeBase64(const char* encoded, size_t encodedLength, uint8_t* decoded,
                  size_t decodedCapacity, size_t& decodedLength) {
  decodedLength = 0;
  if (encoded == nullptr || encodedLength == 0 || encodedLength % 4 != 0) {
    return false;
  }

  for (size_t i = 0; i < encodedLength; i += 4) {
    const bool lastBlock = (i + 4 == encodedLength);
    const int a = base64Value(encoded[i]);
    const int b = base64Value(encoded[i + 1]);
    const int c = encoded[i + 2] == '=' ? 0 : base64Value(encoded[i + 2]);
    const int d = encoded[i + 3] == '=' ? 0 : base64Value(encoded[i + 3]);
    const bool padC = encoded[i + 2] == '=';
    const bool padD = encoded[i + 3] == '=';
    if (a < 0 || b < 0 || c < 0 || d < 0 || (padC && !padD) ||
        ((!lastBlock) && (padC || padD))) {
      return false;
    }

    const size_t bytesInBlock = padC ? 1 : (padD ? 2 : 3);
    if (decodedLength + bytesInBlock > decodedCapacity) {
      return false;
    }
    const uint32_t bits = (static_cast<uint32_t>(a) << 18) |
                          (static_cast<uint32_t>(b) << 12) |
                          (static_cast<uint32_t>(c) << 6) |
                          static_cast<uint32_t>(d);
    decoded[decodedLength++] = static_cast<uint8_t>(bits >> 16);
    if (bytesInBlock > 1) decoded[decodedLength++] = static_cast<uint8_t>(bits >> 8);
    if (bytesInBlock > 2) decoded[decodedLength++] = static_cast<uint8_t>(bits);
  }
  return true;
}

}  // namespace

GeminiClient* GeminiClient::instance_ = nullptr;

GeminiClient::GeminiClient(AudioOutput& audioOutput)
    : audioOutput_(audioOutput), connected_(false), sessionReady_(false), setupSent_(false),
      responseActive_(false) {
  instance_ = this;
}

void GeminiClient::begin() {
  // Blocca il tentativo iniziale quando è ancora presente il valore segnaposto.
  if (GEMINI_API_KEY[0] == '\0' || String(GEMINI_API_KEY) == "YOUR_GEMINI_API_KEY") {
    Serial.println("[WS] Gemini API key is not configured");
    return;
  }

  // L'API usa un endpoint WebSocket bidirezionale autenticato tramite chiave API.
  const String path = String("/ws/google.ai.generativelanguage.v1beta.GenerativeService.BidiGenerateContent?key=") +
                      GEMINI_API_KEY;

  webSocket_.beginSSL("generativelanguage.googleapis.com", 443, path.c_str());
  webSocket_.onEvent(GeminiClient::handleWebSocketEvent);
  webSocket_.setReconnectInterval(5000);
  webSocket_.enableHeartbeat(15000, 3000, 2);

  Serial.println("[WS] Connecting to Gemini Live API");
}

void GeminiClient::update() {
  webSocket_.loop();
}

bool GeminiClient::isConnected() const {
  return connected_;
}

bool GeminiClient::isSessionReady() const {
  return sessionReady_;
}

bool GeminiClient::isResponseActive() const {
  return responseActive_;
}

bool GeminiClient::sendAudio(const uint8_t* data, size_t length) {
  if (!connected_ || !sessionReady_ || data == nullptr || length == 0) {
    return false;
  }

  // I campioni binari PCM vengono trasportati nel campo JSON come base64.
  const String encoded = base64::encode(data, length);

  DynamicJsonDocument message(1024 + encoded.length());
  JsonObject realtimeInput = message["realtimeInput"].to<JsonObject>();
  JsonObject audio = realtimeInput["audio"].to<JsonObject>();
  audio["data"] = encoded;
  audio["mimeType"] = "audio/pcm;rate=16000";

  String serialized;
  serializeJson(message, serialized);
  return webSocket_.sendTXT(serialized);
}

bool GeminiClient::endAudioStream() {
  if (!connected_ || !sessionReady_) {
    return false;
  }

  DynamicJsonDocument message(128);
  message["realtimeInput"]["audioStreamEnd"] = true;
  String serialized;
  serializeJson(message, serialized);
  return webSocket_.sendTXT(serialized);
}

void GeminiClient::disconnect() {
  webSocket_.disconnect();
  connected_ = false;
  sessionReady_ = false;
  setupSent_ = false;
}

void GeminiClient::handleWebSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
  // La libreria WebSocket richiede un callback statico; instance_ inoltra l'evento all'oggetto.
  if (instance_ != nullptr) {
    instance_->onWebSocketEvent(type, payload, length);
  }
}

void GeminiClient::onWebSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_CONNECTED:
      connected_ = true;
      Serial.printf("[WS] Gemini connected at %lu ms\n", millis());
      sendSetup();
      break;
    case WStype_DISCONNECTED:
      connected_ = false;
      sessionReady_ = false;
      setupSent_ = false;
      responseActive_ = false;
      if (payload != nullptr && length > 0) {
        Serial.printf("[WS] Gemini disconnected at %lu ms: %.*s\n", millis(), static_cast<int>(length),
                      reinterpret_cast<const char*>(payload));
      } else {
        Serial.printf("[WS] Gemini disconnected at %lu ms (no reason provided)\n", millis());
      }
      break;
    case WStype_TEXT:
      Serial.printf("[WS] Gemini RX text frame at %lu ms (%u bytes)\n", millis(),
                    static_cast<unsigned int>(length));
      handleServerMessage(payload, length);
      break;
    case WStype_BIN:
      Serial.printf("[WS] Gemini RX binary frame at %lu ms (%u bytes)\n", millis(),
                    static_cast<unsigned int>(length));
      handleServerMessage(payload, length);
      break;
    case WStype_PING:
      Serial.printf("[WS] Gemini ping at %lu ms\n", millis());
      break;
    case WStype_PONG:
      Serial.printf("[WS] Gemini pong at %lu ms\n", millis());
      break;
    case WStype_ERROR:
      if (payload != nullptr && length > 0) {
        Serial.printf("[WS] Gemini WebSocket error: %.*s\n", static_cast<int>(length),
                      reinterpret_cast<const char*>(payload));
      } else {
        Serial.println("[WS] Gemini WebSocket error (no details provided)");
      }
      break;
    default:
      break;
  }
}

void GeminiClient::sendSetup() {
  if (!connected_ || setupSent_) {
    return;
  }

  // Informa il servizio del modello e richiede risposte vocali prima di inviare l'audio.
  DynamicJsonDocument setup(1024);
  JsonObject setupObject = setup["setup"].to<JsonObject>();
  setupObject["model"] = GEMINI_MODEL;
  JsonObject generationConfig = setupObject["generationConfig"].to<JsonObject>();
  JsonArray responseModalities = generationConfig["responseModalities"].to<JsonArray>();
  responseModalities.add("AUDIO");
  setupObject["inputAudioTranscription"].to<JsonObject>();
  setupObject["outputAudioTranscription"].to<JsonObject>();

  String serialized;
  serializeJson(setup, serialized);
  // setupSent_ evita invii duplicati durante la stessa connessione.
  if (webSocket_.sendTXT(serialized)) {
    setupSent_ = true;
    sessionReady_ = false;
    Serial.println("[AI] Session setup sent; waiting for setupComplete");
  }
}

void GeminiClient::handleServerMessage(const uint8_t* payload, size_t length) {
  // Analizza solo i messaggi JSON testuali e ignora quelli non validi.
  DynamicJsonDocument message(8192);
  const DeserializationError error = deserializeJson(message, payload, length);
  if (error) {
    Serial.printf("[WS] Gemini RX JSON parse failed (%s, %u bytes, doc capacity %u)\n",
                  error.c_str(), static_cast<unsigned int>(length),
                  static_cast<unsigned int>(message.capacity()));
    return;
  }

  Serial.printf("[WS] Gemini RX JSON parsed (%u bytes)\n", static_cast<unsigned int>(length));

  JsonObject usage = message["usageMetadata"].as<JsonObject>();
  if (!usage.isNull()) {
    Serial.printf("[AI] Usage: prompt=%u response=%u total=%u\n",
                  static_cast<unsigned int>(usage["promptTokenCount"] | 0),
                  static_cast<unsigned int>(usage["responseTokenCount"] | 0),
                  static_cast<unsigned int>(usage["totalTokenCount"] | 0));
  }

  JsonObject apiError = message["error"].as<JsonObject>();
  if (!apiError.isNull()) {
    const char* status = apiError["status"] | "unknown";
    const char* details = apiError["message"] | "No message provided";
    Serial.printf("[WS] Gemini API error (%s, code %d): %.180s\n", status,
                  apiError["code"] | 0, details);
    return;
  }

  // Registra la conferma esplicita del server che il setup della sessione è completo.
  if (message["setupComplete"].is<JsonObject>()) {
    sessionReady_ = true;
    Serial.println("[AI] Session initialized");
  }

  JsonObject serverContent = message["serverContent"].as<JsonObject>();
  if (!serverContent.isNull()) {
    const char* inputTranscript = serverContent["inputTranscription"]["text"] | nullptr;
    if (inputTranscript != nullptr) {
      Serial.printf("[AI] Input transcription received (%u characters)\n",
                    static_cast<unsigned int>(strlen(inputTranscript)));
    }
    const char* outputTranscript = serverContent["outputTranscription"]["text"] | nullptr;
    if (outputTranscript != nullptr) {
      Serial.printf("[AI] Output transcription received (%u characters)\n",
                    static_cast<unsigned int>(strlen(outputTranscript)));
    }

    JsonArray parts = serverContent["modelTurn"]["parts"].as<JsonArray>();
    Serial.printf("[AI] Server content: %u parts, turnComplete=%s, interrupted=%s\n",
                  static_cast<unsigned int>(parts.size()),
                  serverContent["turnComplete"].as<bool>() ? "true" : "false",
                  serverContent["interrupted"].as<bool>() ? "true" : "false");

    if (serverContent["interrupted"].as<bool>()) {
      responseActive_ = false;
      audioOutput_.clear();
      Serial.println("[AI] Response interrupted");
    }

    size_t audioChunks = 0;
    size_t encodedAudioBytes = 0;
    size_t decodedAudioBytes = 0;
    size_t writtenAudioSamples = 0;
    for (JsonObject part : parts) {
      JsonObject inlineData = part["inlineData"].as<JsonObject>();
      const char* mimeType = inlineData["mimeType"] | "(missing)";
      const char* encodedAudio = inlineData["data"] | nullptr;
      if (encodedAudio != nullptr) {
        const size_t encodedLength = strlen(encodedAudio);
        ++audioChunks;
        encodedAudioBytes += encodedLength;
        Serial.printf("[AI] Audio chunk received: mime=%s, base64=%u bytes\n", mimeType,
                      static_cast<unsigned int>(encodedLength));
        const size_t decodedCapacity = (encodedLength / 4) * 3;
        uint8_t* decoded = static_cast<uint8_t*>(malloc(decodedCapacity));
        size_t decodedLength = 0;
        if (decoded == nullptr ||
            !decodeBase64(encodedAudio, encodedLength, decoded, decodedCapacity, decodedLength)) {
          Serial.println("[AI] Audio chunk Base64 decode failed");
          free(decoded);
          continue;
        }

        // Gemini invia PCM mono signed 16-bit little-endian a 24 kHz.
        decodedAudioBytes += decodedLength;
        const size_t sampleCount = decodedLength / sizeof(int16_t);
        if (sampleCount > 0 && audioOutput_.isInitialized()) {
          responseActive_ = true;
          const size_t written = audioOutput_.write(reinterpret_cast<const int16_t*>(decoded), sampleCount);
          writtenAudioSamples += written;
          Serial.printf("[AI] I2S TX chunk: %u/%u samples written\n",
                        static_cast<unsigned int>(written), static_cast<unsigned int>(sampleCount));
        } else if (!audioOutput_.isInitialized()) {
          Serial.println("[AI] Audio received but I2S output is unavailable");
        }
        free(decoded);
      }
    }

    if (audioChunks > 0) {
      Serial.printf("[AI] Audio routing summary: %u chunks, %u base64 bytes, %u PCM bytes, %u/%u samples to I2S\n",
                    static_cast<unsigned int>(audioChunks),
                    static_cast<unsigned int>(encodedAudioBytes),
                    static_cast<unsigned int>(decodedAudioBytes),
                    static_cast<unsigned int>(writtenAudioSamples),
                    static_cast<unsigned int>(decodedAudioBytes / sizeof(int16_t)));
    } else if (parts.size() > 0) {
      Serial.println("[AI] Model parts received, but no inlineData.audio chunk was found");
    }

    if (serverContent["turnComplete"].as<bool>()) {
      responseActive_ = false;
      Serial.println("[AI] Response complete");
    }
  }
}

}  // namespace ai_companion
