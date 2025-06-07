#include "Jab2D_Avnd.hpp"

namespace puara_gestures::objects
{

void Jab2D_Avnd::operator()()
{

  const puara_gestures::Coord2D& current_accel_2d = inputs.acceleration_2d.value;
  const float desired_threshold_float = inputs.threshold_param.value;

  const int desired_threshold_int = static_cast<int>(desired_threshold_float);
  if(impl.x.threshold != desired_threshold_int)
  {
    impl.x.threshold = desired_threshold_int;
  }
  if(impl.y.threshold != desired_threshold_int)
  {
    impl.y.threshold = desired_threshold_int;
  }
  // puara_gestures::Jab2D itself does not have a single 'threshold' member.

  // Calling the update method of the puara_gestures::Jab2D implementation
  impl.update(current_accel_2d.x, current_accel_2d.y);

  // Getting the calculated 2D jab vector
  const puara_gestures::Coord2D calculated_jab_vector = impl.current_value();

  outputs.output_2d.value = calculated_jab_vector;
}

}
