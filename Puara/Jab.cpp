#include "Jab.hpp"

namespace puara_gestures::objects
{
void Jab::operator()(halp::tick t)
{
  // 1. input values
  const puara_gestures::Coord3D& current_accel = inputs.accel_3D.value;
  const float desired_threshold_float = inputs.jab_threshold.value;
  int desired_threshold_int = static_cast<int>(desired_threshold_float);

  // 2. Updateing parameters of the underlying puara_gestures::Jab objects

  if (impl.x.threshold != desired_threshold_int) {
    impl.x.threshold = desired_threshold_int;
  }
  if (impl.y.threshold != desired_threshold_int) {
    impl.y.threshold = desired_threshold_int;
  }
  if (impl.z.threshold != desired_threshold_int) {
    impl.z.threshold = desired_threshold_int;
  }

  // 3. calling  Jab3D impl.update()
  impl.update(current_accel.x, current_accel.y, current_accel.z);

  // 4. getting  the calculated 3D jab vector
  puara_gestures::Coord3D calculated_jab_vector = impl.current_value();

  // 5. Setting the 3D output port
  outputs.output_3d.value = calculated_jab_vector;
}
}
