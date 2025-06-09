#include "Jab.hpp"

namespace puara_gestures::objects
{

void Jab::operator()()
{
  // 1. Get input values
  const puara_gestures::Coord3D& current_accel = inputs.accel_3D;
  const float desired_threshold_float = inputs.jab_threshold;

  // 2. Update parameters of the underlying puara_gestures::Jab objects
  const int threshold_to_set = desired_threshold_float;
  impl.x.threshold = threshold_to_set;
  impl.y.threshold = threshold_to_set;
  impl.z.threshold = threshold_to_set;

  impl.update(current_accel.x, current_accel.y, current_accel.z);

  // 4. Getting the calculated 3D jab vec ...
  const auto calculated_jab_vector = impl.current_value();

  // 5. Setting the 3D output port
  outputs.output_3d = calculated_jab_vector;
}

}
