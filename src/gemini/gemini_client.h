#pragma once

#include <Arduino.h>
#include <WebSocketsClient.h>

namespace ai_companion {

class GeminiClient {
 public:
  GeminiClient();

  // Avvia il collegamento WSS; la sessione viene configurata dopo l'evento CONNECTED.
  void begin();
  // Fa avanzare il client WebSocket e processa gli eventi in arrivo.
  void update();
  bool isConnected() const;
  bool isSessionReady() const;
  // Invia PCM grezzo codificato in base64 solo quando la sessione è pronta.
  bool sendAudio(const uint8_t* data, size_t length);
  void disconnect();

 private:
  static void handleWebSocketEvent(WStype_t type, uint8_t* payload, size_t length);
  static GeminiClient* instance_;

  void onWebSocketEvent(WStype_t type, uint8_t* payload, size_t length);
  void handleServerMessage(const uint8_t* payload, size_t length);
  void sendSetup();

  WebSocketsClient webSocket_;
  bool connected_;
  bool sessionReady_;
  bool setupSent_;
};

}  // namespace ai_companion
