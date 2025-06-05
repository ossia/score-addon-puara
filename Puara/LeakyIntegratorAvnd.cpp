#include "LeakyIntegratorAvnd.hpp"

namespace puara_gestures::objects
{

void LeakyIntegratorAvnd::operator()(halp::tick t)
{
  const float current_input_signal = inputs.input_value.value;
  const float current_leak_factor  = inputs.leak_param.value;

  double output_val = impl.integrate(
      static_cast<double>(current_input_signal),
      static_cast<double>(current_leak_factor)
      );


  outputs.output_value.value = static_cast<float>(output_val);

}

}
