#include "Jab.hpp"
namespace puara_gestures::objects
{
void Jab::operator()(halp::tick t) // for my info t contain info like t.frames
{
  // get input data (avendish)

  const puara_gestures::Coord3D & current_accel = inputs.accel.value;
  printf("Accel: x=%.2f, y=%.2f, z=%.2f\n", current_accel.x, current_accel.y, current_accel.z);


  //2.updating the  puara gestues implementation
  // imlp is now type Jab3D with method of update that takes x,y,z components as referred from puara/descriptors/jab.h

  impl.update(current_accel.x, current_accel.y, current_accel.z);


  //3. getting the processed value from puara
  // as learned impl.current_value() for jab3d return coord3d representing jab intensities on each axis
  puara_gestures::Coord3D jab_vector = impl.current_value();
  printf("Puara Jab Vector: x=%.2f, y=%.2f, z=%.2f\n", jab_vector.x, jab_vector.y, jab_vector.z);

  //4. calculating the desired single fload value / magnitude = sqrt(x^2 + y^2 + z^2)

  float jab_magnitude = std::sqrt(
      (jab_vector.x * jab_vector.x) +
      (jab_vector.y * jab_vector.y) +
      (jab_vector.z * jab_vector.z));

  //5. setting output float value

  outputs.output.value = jab_magnitude;
  printf("Calculated Jab Magnitude: %.2f\n", jab_magnitude);





  // TODO
  // outputs.output = impl.jab(inputs.accel, inputs.gyro, inputs.mag, 0.1);
}
}
