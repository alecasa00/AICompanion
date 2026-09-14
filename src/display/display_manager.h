#pragma once

#include <Adafruit_SSD1306.h>

namespace ai_companion {

class DisplayManager {
 public:
  DisplayManager();
  bool begin();
  void showState(const char* stateName);
  bool isInitialized() const;

 private:
  Adafruit_SSD1306 display_;
  bool initialized_ = false;
};

}  // namespace ai_companion