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
  } inputs;

  struct
  {
    halp::val_port<"Output", float> output_value;
  } outputs;

  using tick = halp::tick;
  void operator()(halp::tick t);
  puara_gestures::utils::LeakyIntegrator impl{0.0, 0.0, 0.5, 0, 0};

  LeakyIntegratorAvnd() = default;
};

}
