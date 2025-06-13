#include "Jab2D_Avnd.hpp"

namespace puara_gestures::objects
{

void Jab2D_Avnd::operator()()
{

  const puara_gestures::Coord2D& current_accel_2d = inputs.acceleration_2d;
  const float desired_threshold_float = inputs.threshold_param;

  const int threshold_to_set = desired_threshold_float;
  impl.x.threshold = threshold_to_set;
  impl.y.threshold = threshold_to_set;

  impl.update(current_accel_2d.x, current_accel_2d.y);

  const auto calculated_jab_vector = impl.current_value();

  outputs.output_2d = calculated_jab_vector;
}

}
