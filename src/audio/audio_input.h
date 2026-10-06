#pragma once

#include <Arduino.h>
#include <driver/i2s.h>

namespace ai_companion {

class AudioInput {
 public:
  AudioInput();

  // Configura il driver I2S in ricezione; false indica pin mancanti o errore del driver.
  bool begin();
  bool isInitialized() const;
  // Legge al massimo maxSamples campioni PCM a 16 bit e restituisce quanti ne ha ottenuti.
  size_t read(int16_t* buffer, size_t maxSamples);
  // Arresta e rimuove il driver I2S se era stato inizializzato.
  void stop();

 private:
  i2s_port_t port_;
  bool initialized_;
};

}  // namespace ai_companion
