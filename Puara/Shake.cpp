#include "Shake.hpp"
namespace puara_gestures::objects
{
void Shake::operator()(halp::tick t)
{

  // getting input data

  const puara_gestures::Coord3D& current_accel = inputs.accel.value;
  const float desired_frequency_hz = inputs.integrator_frequency.value;
  const float desired_fast_leak = inputs.fast_leak_param.value;
  const float desired_slow_leak = inputs.slow_leak_param.value;
  const float desired_activation_threshold = inputs.activation_threshold_param.value;

  // updating puara parameters

  //checking if the desired frequency for the integrator has changed
  int desired_frequency_int = static_cast<int>(desired_frequency_hz);

  //updating freq for all axes
  if(impl.x.integrator.frequency != desired_frequency_int)
  {
    impl.frequency(static_cast<double>(desired_frequency_int));
  }

  //updating leak params for each axis
  impl.x.fast_leak = static_cast<double>(desired_fast_leak);
  impl.x.slow_leak = static_cast<double>(desired_slow_leak);

  impl.y.fast_leak = static_cast<double>(desired_fast_leak);
  impl.y.slow_leak = static_cast<double>(desired_slow_leak);

  impl.z.fast_leak = static_cast<double>(desired_fast_leak);
  impl.z.slow_leak = static_cast<double>(desired_slow_leak);

  //3.updating the puara gestures implementation
  impl.update(current_accel.x, current_accel.y, current_accel.z);

  //4.getting processed value from puara
  puara_gestures::Coord3D shake_vector = impl.current_value();

  //5. calculating the desired float value

  float shake_magnitude = std::sqrt(
      (shake_vector.x * shake_vector.x) + (shake_vector.y * shake_vector.y)
      + (shake_vector.z * shake_vector.z));

  //6 setting the output port value

  outputs.output.value = shake_magnitude;
}
}
