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
  const unsigned long now = millis();
  const wl_status_t status = WiFi.status();

  // Rileva la connessione anche mentre WiFi.begin() è ancora in corso.
  // Se il collegamento cade in seguito, consente invece di avviare un nuovo tentativo.
  if (status == WL_CONNECTED) {
    if (!connected_) {
      connected_ = true;
      attemptInProgress_ = false;
      lastError_ = "CONNECTED";
      Serial.print("[WIFI] Connected: ");
      Serial.println(WiFi.localIP());
    }
    return;
  }

  connected_ = false;

  // Al timeout termina il tentativo corrente e ne avvia uno nuovo.
  if (attemptInProgress_ && now - connectStartedAtMs_ > ai_companion::kWifiTimeoutMs) {
    Serial.println("[WIFI] Connection timed out");
    lastError_ = "CONNECTION_TIMEOUT";
    WiFi.disconnect();
    attemptInProgress_ = false;
    connect();
    return;
  }

  // Se non c'è un tentativo attivo, controlla periodicamente se occorre riconnettersi.
  if (now - lastStatusCheckMs_ > 1000) {
    lastStatusCheckMs_ = now;

    if (!attemptInProgress_ && (status == WL_IDLE_STATUS || status == WL_NO_SSID_AVAIL ||
                                status == WL_CONNECT_FAILED || status == WL_DISCONNECTED)) {
      connect();
    }
  }
}

void WifiManager::connect() {
  // Memorizza l'istante di avvio per poter misurare il timeout in update().
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
