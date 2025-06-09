#include "LeakyIntegratorAvnd.hpp"

namespace puara_gestures::objects
{

void LeakyIntegratorAvnd::operator()(halp::tick t)
{
  const float current_input_signal = inputs.input_value.value;
  const float current_leak_factor = inputs.leak_param.value;
  const float current_leak_frequency_hz = inputs.leak_frequency.value;

  const int desired_frequency_int = current_leak_frequency_hz;
  if(impl.frequency != desired_frequency_int)
  {
    impl.frequency = desired_frequency_int;
  }

  const double output_val = impl.integrate(current_input_signal, current_leak_factor);

  outputs.output_value.value = output_val;
}

}
