#pragma once

#include <halp/audio.hpp>
#include <halp/controls.hpp>
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
  halp_meta(uuid, "ed55a080-b821-4977-89b6-4697cb8178e7")

  struct ins
  {
    halp::val_port<"Input", float> input_value;
    // Leak factor (0.0 = full leak this step, 1.0 = no leak this step)
    halp::knob_f32<"Leak Factor", halp::range{0.0, 1.0, 0.5}> leak_param;
    // input port for leaking frequency  in hz
    // range 0 Hz to 200 Hz def 100 Hz
    // if freq is 0  will bypass timed leaking, using only leak_param per integrate call.
    halp::knob_f32<"Leak Frequency (Hz)",halp::range{0.0,200.0,100.0}>leak_freqency;
  } inputs;

  struct outputs
  {
    halp::val_port<"Output", float> output_value;
  } outputs;

  using tick = halp::tick;
  void operator()(halp::tick t);

  // initializing impl with default values  inc  a def freq
  puara_gestures::utils::LeakyIntegrator impl{
      0.0, // initial currentValue
      0.0, // initial oldValue
      0.5, // initial leak (will be --> overridden by --> leak_param port)
      100, // initial frequency in Hz --> (will be overridden by --> leak_frequency port)
      0    // initial timerValue
  };

  LeakyIntegratorAvnd() = default;
};

}
