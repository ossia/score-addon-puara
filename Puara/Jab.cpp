#include "Jab.hpp"
namespace puara_gestures::objects
{
void Jab::operator()(halp::tick t)
{

  //1.  input
  const puara_gestures::Coord3D& current_accel = inputs.accel.value;
  //2. calling jab 3d impl.update() -->leading to updating the  internel x,y,z jab objects within 'impl'
  impl.update(current_accel.x, current_accel.y, current_accel.z);

  //3. getting the  calculated jab vecotr
  puara_gestures::Coord3D calculated_jab_vector = impl.current_value();

  //4.setting the output port

  outputs.output.value = calculated_jab_vector;


}
}
