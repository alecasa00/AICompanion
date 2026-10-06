#pragma once

#include <Arduino.h>
#include <driver/i2s.h>

namespace ai_companion {

class AudioOutput {
 public:
  AudioOutput();

  // Configura il driver I2S in trasmissione verso l'amplificatore.
  bool begin();
  bool isInitialized() const;
  // Scrive campioni PCM a 16 bit e restituisce quanti campioni sono stati accettati.
  size_t write(const int16_t* buffer, size_t sampleCount);
  // Arresta e rimuove il driver I2S se era stato inizializzato.
  void stop();

 private:
  i2s_port_t port_;
  bool initialized_;
};

}  // namespace ai_companion
