#include "Shake.hpp"

#include <cmath>

namespace puara_gestures::objects
{

void Shake::operator()()
{

  const puara_gestures::Coord3D& current_accel = inputs.accel;
  const float desired_frequency_hz = inputs.integrator_frequency;
  const float desired_fast_leak = inputs.fast_leak_param;
  const float desired_slow_leak = inputs.slow_leak_param;

  const int desired_frequency_int = desired_frequency_hz;
  if(impl.x.integrator.frequency != desired_frequency_int)
  {
    impl.frequency(desired_frequency_int);
  }

  impl.x.fast_leak = desired_fast_leak;
  impl.x.slow_leak = desired_slow_leak;
  impl.y.fast_leak = desired_fast_leak;
  impl.y.slow_leak = desired_slow_leak;
  impl.z.fast_leak = desired_fast_leak;
  impl.z.slow_leak = desired_slow_leak;

  impl.update(current_accel.x, current_accel.y, current_accel.z);

  const auto shake_vector = impl.current_value();

  const float shake_magnitude
      = std::hypot(shake_vector.x, shake_vector.y, shake_vector.z);

  outputs.output = shake_magnitude;
}

}
