#pragma once
#include <Wire.h>
#include <AudioTools/AudioLibs/I2SCodecStream.h>
#include "FreenovePins.h"

class FreenoveAudioPins : public audio_driver::DriverPins {
public:
  FreenoveAudioPins() {
    // Wire is initialized by setup() and remains alive for the touch controller.
    addI2C(audio_driver::PinFunction::CODEC, Wire, false);
    addI2S(audio_driver::PinFunction::CODEC, FN_I2S_MCLK, FN_I2S_BCLK,
           FN_I2S_WS, FN_I2S_DOUT, -1);
    addPin(audio_driver::PinFunction::PA, FN_AUDIO_ENABLE,
           audio_driver::PinLogic::OutputActiveLow);
  }
};

// AudioTools 1.2.2 updates its cached format before checking for changes.
// Ensure the codec is reconfigured when the decoder changes sample rate.
class FreenoveAudioStream : public audio_tools::I2SCodecStream {
public:
  using I2SCodecStream::I2SCodecStream;
  void setAudioInfo(audio_tools::AudioInfo info) override {
    const bool changed = cfg.sample_rate != info.sample_rate ||
                         cfg.bits_per_sample != info.bits_per_sample ||
                         cfg.channels != info.channels;
    I2SCodecStream::setAudioInfo(info);
    if (changed && is_active && p_board && !p_board->setConfig(codec_cfg))
      log_e("ES8311 format update failed");
  }
};
