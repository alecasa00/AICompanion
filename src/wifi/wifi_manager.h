#pragma once

#include <Arduino.h>
#include <WiFi.h>

namespace ai_companion {

class WifiManager {
 public:
  WifiManager();

  // Imposta la modalità station e avvia il primo tentativo di connessione.
  void begin();
  // Controlla timeout e stato Wi-Fi senza bloccare il ciclo principale.
  void update();
  bool isConnected() const;
  String ipAddressString() const;
  const char* lastError() const;

 private:
  void connect();

  bool connected_;
  bool attemptInProgress_;
  unsigned long connectStartedAtMs_;
  unsigned long lastStatusCheckMs_;
  String lastError_;
};

}  // namespace ai_companion
