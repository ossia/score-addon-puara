#include "Jab1D_Avnd.hpp"

namespace puara_gestures::objects
{

void Jab1D_Avnd::operator()()
{

  const float current_accel_1d = inputs.acceleration_1d.value;
  const float desired_threshold_float = inputs.threshold_param.value;

  const int desired_threshold_int = static_cast<int>(desired_threshold_float);
  if(impl.threshold != desired_threshold_int)
  {
    impl.threshold = desired_threshold_int;
  }
  const double jab_value = impl.update(static_cast<double>(current_accel_1d));

  outputs.output_1d.value = static_cast<float>(jab_value);
}

}
