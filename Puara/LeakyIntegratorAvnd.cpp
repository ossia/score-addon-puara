#include "LeakyIntegratorAvnd.hpp"

namespace puara_gestures::objects
{
void LeakyIntegratorAvnd::operator()()
{
  const float current_input_signal = inputs.input_value;
  const float current_leak_factor = inputs.leak_param;
  const float current_leak_frequency_hz = inputs.leak_frequency;
  const int desired_frequency_int = current_leak_frequency_hz;

  impl.frequency = desired_frequency_int;

  const double output_val = impl.integrate(current_input_signal, current_leak_factor);

  outputs.output_value = output_val;
}

}
