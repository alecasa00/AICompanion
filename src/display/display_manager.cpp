#include "display_manager.h"

#include <Wire.h>

#include "../config/config.h"

namespace ai_companion {

DisplayManager::DisplayManager()
    : display_(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET_PIN) {}

bool DisplayManager::begin() {
  if (OLED_SDA_PIN < 0 || OLED_SCL_PIN < 0) {
    Serial.println("[OLED] SDA/SCL pins are not configured");
    return false;
  }

  Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN);
  initialized_ = display_.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDRESS);
  if (!initialized_) {
    Serial.printf("[OLED] Initialization failed at 0x%02X\n", OLED_I2C_ADDRESS);
    return false;
  }

  display_.clearDisplay();
  display_.setTextColor(SSD1306_WHITE);
  display_.setTextSize(1);
  display_.display();
  Serial.printf("[OLED] Ready at 0x%02X\n", OLED_I2C_ADDRESS);
  return true;
}

void DisplayManager::showState(const char* stateName) {
  if (!initialized_) {
    return;
  }

  display_.clearDisplay();
  display_.setCursor(0, 0);
  display_.println("AICompanion");
  display_.drawFastHLine(0, 10, OLED_WIDTH, SSD1306_WHITE);
  display_.setCursor(0, 24);
  display_.println("SYSTEM STATE");
  display_.setCursor(0, 40);
  display_.println(stateName);
  display_.display();
}

bool DisplayManager::isInitialized() const {
  return initialized_;
}

}  // namespace ai_companion