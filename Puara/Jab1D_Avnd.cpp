#include "Jab1D_Avnd.hpp"

namespace puara_gestures::objects
{

void Jab1D_Avnd::operator()()
{

  const float current_accel_1d = inputs.acceleration_1d;
  const float desired_threshold_float = inputs.threshold_param;

  impl.threshold = desired_threshold_float;

  //    'current_accel_1d' (float) will implicitly promote to double for impl.update(double).
  const double jab_value_double = impl.update(current_accel_1d);

  outputs.output_1d = jab_value_double;
}

}
