#include "audio/audio_input.h"

#include "config/config.h"

namespace ai_companion {

AudioInput::AudioInput() : port_(I2S_NUM_0), initialized_(false) {}

bool AudioInput::begin() {
  if (AUDIO_MIC_BCLK_PIN < 0 || AUDIO_MIC_WS_PIN < 0 || AUDIO_MIC_DATA_IN_PIN < 0) {
    Serial.println("[TODO] Microphone I2S pins not configured. See UNCERTAINTIES.txt and docs/hardware.md.");
    return false;
  }

  i2s_config_t config = {
      .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
      .sample_rate = AUDIO_INPUT_SAMPLE_RATE,
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
      .mck_io_num = AUDIO_MIC_MCLK_PIN,
      .bck_io_num = AUDIO_MIC_BCLK_PIN,
      .ws_io_num = AUDIO_MIC_WS_PIN,
      .data_out_num = I2S_PIN_NO_CHANGE,
      .data_in_num = AUDIO_MIC_DATA_IN_PIN,
  };

  esp_err_t err = i2s_driver_install(port_, &config, 0, nullptr);
  if (err != ESP_OK) {
    Serial.printf("[AUDIO] I2S install failed: %d\n", err);
    return false;
  }

  err = i2s_set_pin(port_, &pins);
  if (err != ESP_OK) {
    Serial.printf("[AUDIO] I2S pin config failed: %d\n", err);
    i2s_driver_uninstall(port_);
    return false;
  }

  initialized_ = true;
  Serial.println("[AUDIO] Input initialized");
  return true;
}

bool AudioInput::isInitialized() const {
  return initialized_;
}

size_t AudioInput::read(int16_t* buffer, size_t maxSamples) {
  if (!initialized_ || buffer == nullptr || maxSamples == 0) {
    return 0;
  }

  size_t bytesRead = 0;
  i2s_read(port_, buffer, maxSamples * sizeof(int16_t), &bytesRead, portMAX_DELAY);
  return bytesRead / sizeof(int16_t);
}

void AudioInput::stop() {
  if (initialized_) {
    i2s_stop(port_);
    i2s_driver_uninstall(port_);
    initialized_ = false;
  }
}

}  // namespace ai_companion
