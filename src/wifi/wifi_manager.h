#pragma once

#include <Arduino.h>
#include <WiFi.h>

namespace ai_companion {

class WifiManager {
 public:
  WifiManager();

  void begin();
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
