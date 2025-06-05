#include "LeakyIntegratorAvnd.hpp" // Your Avendish processor header

namespace puara_gestures::objects
{

void LeakyIntegratorAvnd::operator()(halp::tick t)
{
  // 1. Get input values from Avendish ports
  const float current_input_signal = inputs.input_value.value;
  const float current_leak_factor  = inputs.leak_param.value;

  double output_val = impl.integrate(
      static_cast<double>(current_input_signal),
      static_cast<double>(current_leak_factor) // This is the 'leakValue' for the call
      // which internally uses impl.old_value, impl.frequency (0), and impl.timer
      );

  // 3. Setting the Avendish output port value
  outputs.output_value.value = static_cast<float>(output_val);

  printf("LeakyAvnd: In=%.2f, LeakP=%.2f -> Out=%.2f (impl.old_val=%.2f, impl.curr_val=%.2f)\n",
         current_input_signal, current_leak_factor, output_val, impl.old_value, impl.current_value);
}

}
