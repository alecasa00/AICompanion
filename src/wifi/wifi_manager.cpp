#include "wifi/wifi_manager.h"

#include "config/config.h"

namespace ai_companion {

WifiManager::WifiManager()
    : connected_(false),
      attemptInProgress_(false),
      connectStartedAtMs_(0),
      lastStatusCheckMs_(0),
      lastError_("NO_ERROR") {}

void WifiManager::begin() {
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.persistent(false);

  connect();
}

void WifiManager::update() {
  if (connected_) {
    return;
  }

  const unsigned long now = millis();

  if (attemptInProgress_ && now - connectStartedAtMs_ > ai_companion::kWifiTimeoutMs) {
    Serial.println("[WIFI] Connection timed out");
    lastError_ = "CONNECTION_TIMEOUT";
    WiFi.disconnect();
    attemptInProgress_ = false;
    connect();
    return;
  }

  if (!attemptInProgress_ && now - lastStatusCheckMs_ > 1000) {
    lastStatusCheckMs_ = now;
    const wl_status_t status = WiFi.status();
    if (status == WL_CONNECTED) {
      connected_ = true;
      lastError_ = "CONNECTED";
      Serial.print("[WIFI] Connected: ");
      Serial.println(WiFi.localIP());
      return;
    }

    if (status == WL_IDLE_STATUS || status == WL_NO_SSID_AVAIL || status == WL_CONNECT_FAILED ||
        status == WL_DISCONNECTED) {
      connect();
    }
  }
}

void WifiManager::connect() {
  attemptInProgress_ = true;
  connectStartedAtMs_ = millis();
  lastStatusCheckMs_ = connectStartedAtMs_;

  Serial.println("[WIFI] Attempting connection...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

bool WifiManager::isConnected() const {
  return connected_ || WiFi.status() == WL_CONNECTED;
}

String WifiManager::ipAddressString() const {
  return WiFi.localIP().toString();
}

const char* WifiManager::lastError() const {
  return lastError_.c_str();
}

}  // namespace ai_companion
