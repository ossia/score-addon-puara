// #pragma once

// #include <halp/audio.hpp>
// #include <halp/controls.hpp>
// #include <halp/meta.hpp>
// #include <puara/utils/leakyintegrator.h>

// namespace Puara
// {

// class LeakyIntegratorAvnd
// {
// public:
//   halp_meta(name, "Leaky Integrator")
//   halp_meta(category, "Gestures")
//   halp_meta(c_name, "puara_leaky_integrator_avnd")
//   halp_meta(uuid, "ed55a080-b821-4977-89b6-4697cb8178e7")

//   struct ins
//   {
//     halp::val_port<"Input", float> input;

//     //leak factor --> 0.0 full leak this step ,,, 1.0 = no leak this step
//     halp::knob_f32<"Leak", halp::range{0.0, 1.0, 0.5}> leak;
//   } inputs;

//   struct
//   {
//     halp::val_port<"Output", float> output;
//   } outputs;

//   using tick = halp::tick_musical;
//   void operator()(halp::tick_musical t)
//   {
//     unsigned long long time = 1e3 * t.position_in_frames / 48000.;
//     outputs.output = impl.integrate(inputs.input.value, 0.5, time);
//   }

//   puara_gestures::utils::LeakyIntegrator impl{0, 0, 0.5, 100, 0};

//   LeakyIntegratorAvnd() = default;
// };

// }

#pragma once

#include <halp/audio.hpp>
#include <halp/controls.hpp>
#include <halp/meta.hpp>
#include <puara/utils/leakyintegrator.h>
#include <algorithm> // For std::clamp

namespace puara_gestures::objects
{

class LeakyIntegratorAvnd
{
public:
  halp_meta(name, "Leaky Integrator")
  halp_meta(category, "Utilities/Puara")
  halp_meta(c_name, "puara_leaky_integrator_avnd") // Added _avnd
  halp_meta(uuid, "ed55a080-b821-4977-89b6-4697cb8178e7") // Your UUID

  struct ins
  {
    halp::val_port<"Input", float> input_value;
    // Leak factor (0.0 = full leak this step, 1.0 = no leak this step)
    halp::knob_f32<"Leak Factor", halp::range{0.0, 1.0, 0.5}> leak_param;
    // We will set the Puara object's internal frequency to 0 to ensure it leaks every tick.
    // If users want time-based leaking, that's a more advanced setup.
  } inputs;

  struct
  {
    halp::val_port<"Output", float> output_value;
  } outputs;

  using tick = halp::tick; // General control rate tick
  void operator()(halp::tick t);

  // Puara's LeakyIntegrator instance
  // Constructor: LeakyIntegrator(currentValue, oldValue, leakValue, freq, timerValue)
  // We'll initialize freq to 0 to make it leak every time integrate() is called.
  // The leakValue here (0.5) is a default for the impl's member, but we'll override
  // it with inputs.leak_param.value in operator().
  puara_gestures::utils::LeakyIntegrator impl{0.0, 0.0, 0.5, 0, 0};

  LeakyIntegratorAvnd() = default;
};

}
