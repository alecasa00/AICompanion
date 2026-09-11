#pragma once

#include <Arduino.h>
#include <driver/i2s.h>

namespace ai_companion {

class AudioInput {
 public:
  AudioInput();

  bool begin();
  bool isInitialized() const;
  size_t read(int16_t* buffer, size_t maxSamples);
  void stop();

 private:
  i2s_port_t port_;
  bool initialized_;
};

}  // namespace ai_companion
