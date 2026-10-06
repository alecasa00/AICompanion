#pragma once

#include <Adafruit_SSD1306.h>

namespace ai_companion {

class DisplayManager {
 public:
  DisplayManager();
  // Inizializza il bus I2C e il display; il firmware può proseguire anche se fallisce.
  bool begin();
  // Aggiorna la schermata con lo stato corrente del sistema.
  void showState(const char* stateName);
  bool isInitialized() const;

 private:
  Adafruit_SSD1306 display_;
  bool initialized_ = false;
};

}  // namespace ai_companion
