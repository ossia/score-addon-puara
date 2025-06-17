#pragma once

#include <halp/audio.hpp>
#include <halp/controls.hpp>
#include <halp/meta.hpp>
#include <puara/utils/leakyintegrator.h>

namespace Puara
{

class LeakyIntegrator
{
public:
  halp_meta(name, "Leaky Integrator")
  halp_meta(category, "Gestures")
  halp_meta(c_name, "puara_leaky_integrator")
  halp_meta(uuid, "ed55a080-b821-4977-89b6-4697cb8178e7")

  struct
  {
    halp::val_port<"Input", float> input;
    halp::knob_f32<"Leak", halp::range{0.0, 1.0, 0.5}> leak;
  } inputs;

  struct
  {
    halp::val_port<"Output", float> output;
  } outputs;

  using tick = halp::tick_musical;
  void operator()(halp::tick_musical t)
  {
    unsigned long long time = 1e3 * t.position_in_frames / 48000.;
    outputs.output = impl.integrate(inputs.input.value, 0.5, time);
  }

  puara_gestures::utils::LeakyIntegrator impl{0, 0, 0.5, 100, 0};
};

}
