#pragma once

#include <halp/audio.hpp>
#include <halp/controls.hpp>
#include <halp/mappers.hpp>
#include <halp/meta.hpp>
#include <puara/utils/leakyintegrator.h>

namespace puara_gestures::objects
{

class LeakyIntegratorAvnd
{
public:
  halp_meta(name, "Leaky Integrator")
  halp_meta(category, "Utilities/Puara")
  halp_meta(c_name, "puara_leaky_integrator_avnd")
  halp_meta(
      description,
      "Applies a leaky integrator to smooth input signals, with real-time control over "
      "leak amount and update frequency.")
  halp_meta(manual_url, "https://github.com/Puara/puara-gestures/")
  halp_meta(uuid, "ed55a080-b821-4977-89b6-4697cb8178e7")

  struct
  {
    halp::val_port<"Input", float> input_value;
    // Leak factor (0.0 = full leak this step, 1.0 = no leak this step)
    halp::knob_f32<"Leak Factor", halp::range{0.0, 1.0, 0.5}> leak_param;

    struct LeakFrequencyParam
        : halp::knob_f32<"Leak Frequency (Hz)", halp::range{0.0, 200.0, 100.0f}>
    {
      using mapper = halp::log_mapper<std::ratio<85, 100>>;
    } leak_frequency;
  } inputs;

  struct
  {
    halp::val_port<"Output", float> output_value;
  } outputs;

  void operator()();
  // initializing impl with default values  inc  a def freq
  puara_gestures::utils::LeakyIntegrator impl{
      0.0, // initial currentValue
      0.0, // initial oldValue
      0.5, // initial leak (will be --> overridden by --> leak_param port)
      100, // initial frequency in Hz --> (will be overridden by --> leak_frequency port)
      0    // initial timerValue
  };
};

}
