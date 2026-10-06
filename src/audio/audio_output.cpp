#include "audio/audio_output.h"

#include "config/config.h"

namespace ai_companion {

AudioOutput::AudioOutput() : port_(I2S_NUM_1), initialized_(false) {}

bool AudioOutput::begin() {
  // Evita di avviare I2S TX se manca uno dei pin essenziali per l'amplificatore.
  if (AUDIO_SPK_BCLK_PIN < 0 || AUDIO_SPK_WS_PIN < 0 || AUDIO_SPK_DATA_OUT_PIN < 0) {
    Serial.println("[TODO] Speaker I2S pins not configured. See UNCERTAINTIES.txt and docs/hardware.md.");
    return false;
  }

  // Configura il bus come master trasmittente con frequenza e formato definiti nel file config.
  i2s_config_t config = {
      .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
      .sample_rate = AUDIO_OUTPUT_SAMPLE_RATE,
      .bits_per_sample = AUDIO_BITS_PER_SAMPLE,
      .channel_format = (AUDIO_CHANNELS == 1) ? I2S_CHANNEL_FMT_ONLY_LEFT : I2S_CHANNEL_FMT_RIGHT_LEFT,
      .communication_format = I2S_COMM_FORMAT_STAND_I2S,
      .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
      .dma_buf_count = 4,
      .dma_buf_len = 256,
      .use_apll = false,
      .tx_desc_auto_clear = false,
      .fixed_mclk = 0,
      .mclk_multiple = I2S_MCLK_MULTIPLE_DEFAULT,
      .bits_per_chan = I2S_BITS_PER_CHAN_DEFAULT};

  i2s_pin_config_t pins = {
      .mck_io_num = AUDIO_SPK_MCLK_PIN,
      .bck_io_num = AUDIO_SPK_BCLK_PIN,
      .ws_io_num = AUDIO_SPK_WS_PIN,
      .data_out_num = AUDIO_SPK_DATA_OUT_PIN,
      .data_in_num = I2S_PIN_NO_CHANGE,
  };

  // Se l'assegnazione dei pin fallisce, rimuove il driver già installato.
  esp_err_t err = i2s_driver_install(port_, &config, 0, nullptr);
  if (err != ESP_OK) {
    Serial.printf("[AUDIO] I2S TX install failed: %d\n", err);
    return false;
  }

  err = i2s_set_pin(port_, &pins);
  if (err != ESP_OK) {
    Serial.printf("[AUDIO] I2S TX pin config failed: %d\n", err);
    i2s_driver_uninstall(port_);
    return false;
  }

  initialized_ = true;
  Serial.println("[AUDIO] Output initialized");
  return true;
}

bool AudioOutput::isInitialized() const {
  return initialized_;
}

size_t AudioOutput::write(const int16_t* buffer, size_t sampleCount) {
  if (!initialized_ || buffer == nullptr || sampleCount == 0) {
    return 0;
  }

  // L'API I2S usa byte; al chiamante viene restituito il numero di campioni scritti.
  size_t bytesWritten = 0;
  const esp_err_t err = i2s_write(port_, buffer, sampleCount * sizeof(int16_t), &bytesWritten,
                                  pdMS_TO_TICKS(ai_companion::kAudioIoTimeoutMs));
  if (err != ESP_OK) {
    Serial.printf("[AUDIO] Output write timeout/error: %d\n", err);
    return 0;
  }
  return bytesWritten / sizeof(int16_t);
}

void AudioOutput::stop() {
  if (initialized_) {
    i2s_stop(port_);
    i2s_driver_uninstall(port_);
    initialized_ = false;
  }
}

}  // namespace ai_companion
