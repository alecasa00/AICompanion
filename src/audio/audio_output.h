#pragma once

#include <Arduino.h>
#include <driver/i2s.h>

namespace ai_companion {

class AudioOutput {
 public:
  AudioOutput();

  bool begin();
  bool isInitialized() const;
  size_t write(const int16_t* buffer, size_t sampleCount);
  void stop();

 private:
  i2s_port_t port_;
  bool initialized_;
};

}  // namespace ai_companion
