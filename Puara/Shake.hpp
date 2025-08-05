#pragma once

#include <halp/audio.hpp>
#include <halp/controls.hpp>
#include <halp/mappers.hpp>
#include <halp/meta.hpp>
#include <puara/descriptors/shake.h>

namespace puara_gestures::objects
{
class Shake
{
public:
  halp_meta(name, "Shake")
  halp_meta(category, "Analysis/Gestures")
  halp_meta(c_name, "puara_shake")
  halp_meta(
      description,
      "Detects shaking gestures from 3D accelerometer data. Outputs shake magnitude. "
      "Includes configurable integrator frequency, leak factors, and activation "
      "threshold.")
  halp_meta(manual_url, "https://github.com/Puara/puara-gestures/")
  halp_meta(uuid, "ecb5dc46-164c-46a0-a01e-90e3c3bd7527")

  struct
  {
    halp::val_port<"Acceleration", puara_gestures::Coord3D> accel;

    struct IntegratorFrequencyParam
        : halp::knob_f32<"Integrator Frequency (Hz)", halp::range{0.0, 200.0, 10.0f}>
    {
      using mapper = halp::log_mapper<std::ratio<55, 100>>;
    } integrator_frequency;

    halp::knob_f32<"Fast Leak", halp::range{0.0, 1.0, 0.6}> fast_leak_param;
    halp::knob_f32<"Slow Leak", halp::range{0.0, 1.0, 0.3}> slow_leak_param;
    halp::knob_f32<"Activation Threshold", halp::range{0.0, 1.0, 0.1}>
        activation_threshold_param;

  } inputs;

  struct
  {
    halp::val_port<"Output", float> output;
  } outputs;

  void operator()();

  puara_gestures::Shake3D impl;
};

}
