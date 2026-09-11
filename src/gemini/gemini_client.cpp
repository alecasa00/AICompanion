#include "gemini/gemini_client.h"

#include <ArduinoJson.h>
#include <base64.h>

#include "config/config.h"

namespace ai_companion {

GeminiClient* GeminiClient::instance_ = nullptr;

GeminiClient::GeminiClient()
    : connected_(false), sessionReady_(false), setupSent_(false) {
  instance_ = this;
}

void GeminiClient::begin() {
  if (GEMINI_API_KEY[0] == '\0' || String(GEMINI_API_KEY) == "YOUR_GEMINI_API_KEY") {
    Serial.println("[WS] Gemini API key is not configured");
    return;
  }

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

bool GeminiClient::sendAudio(const uint8_t* data, size_t length) {
  if (!connected_ || !sessionReady_ || data == nullptr || length == 0) {
    return false;
  }

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

void GeminiClient::disconnect() {
  webSocket_.disconnect();
  connected_ = false;
  sessionReady_ = false;
  setupSent_ = false;
}

void GeminiClient::handleWebSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
  if (instance_ != nullptr) {
    instance_->onWebSocketEvent(type, payload, length);
  }
}

void GeminiClient::onWebSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_CONNECTED:
      connected_ = true;
      Serial.println("[WS] Gemini connected");
      sendSetup();
      break;
    case WStype_DISCONNECTED:
      connected_ = false;
      sessionReady_ = false;
      setupSent_ = false;
      Serial.println("[WS] Gemini disconnected");
      break;
    case WStype_TEXT:
      handleServerMessage(payload, length);
      break;
    case WStype_ERROR:
      Serial.println("[WS] Gemini WebSocket error");
      break;
    default:
      break;
  }
}

void GeminiClient::sendSetup() {
  if (!connected_ || setupSent_) {
    return;
  }

  DynamicJsonDocument setup(1024);
  JsonObject setupObject = setup["setup"].to<JsonObject>();
  setupObject["model"] = GEMINI_MODEL;
  JsonArray responseModalities = setupObject["responseModalities"].to<JsonArray>();
  responseModalities.add("AUDIO");
  JsonObject systemInstruction = setupObject["systemInstruction"].to<JsonObject>();
  JsonArray parts = systemInstruction["parts"].to<JsonArray>();
  parts.add<JsonObject>()["text"] = "You are a helpful voice assistant.";

  String serialized;
  serializeJson(setup, serialized);
  if (webSocket_.sendTXT(serialized)) {
    setupSent_ = true;
    sessionReady_ = true;
    Serial.println("[AI] Session setup sent");
  }
}

void GeminiClient::handleServerMessage(const uint8_t* payload, size_t length) {
  DynamicJsonDocument message(8192);
  const DeserializationError error = deserializeJson(message, payload, length);
  if (error) {
    Serial.println("[WS] Invalid Gemini JSON message");
    return;
  }

  if (message["setupComplete"].is<JsonObject>()) {
    sessionReady_ = true;
    Serial.println("[AI] Session initialized");
  }

  JsonObject serverContent = message["serverContent"].as<JsonObject>();
  if (!serverContent.isNull()) {
    if (serverContent["interrupted"].as<bool>()) {
      Serial.println("[AI] Response interrupted");
    }

    JsonArray parts = serverContent["modelTurn"]["parts"].as<JsonArray>();
    for (JsonObject part : parts) {
      const char* encodedAudio = part["inlineData"]["data"] | nullptr;
      if (encodedAudio != nullptr) {
        Serial.printf("[AI] Received audio chunk (%u base64 chars)\n",
                      static_cast<unsigned int>(strlen(encodedAudio)));
      }
    }
  }
}

}  // namespace ai_companion
