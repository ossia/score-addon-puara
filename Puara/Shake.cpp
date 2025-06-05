#include "Shake.hpp"
namespace puara_gestures::objects
{
void Shake::operator()(halp::tick t)
{

  // getting input data

  const puara_gestures::Coord3D & current_accel = inputs.accel.value;
  const float desired_frequency = inputs.integrator_frequency.value;

  // updating puara parameters

  //checking if the desired frequency for the integrator has changed
  if(impl.x.frequency()!= desired_frequency){
    impl.frequency(desired_frequency); // setting the freq for x,y,z
  }


  //3.updating the puara gestures implementation
  impl.update(current_accel.x,current_accel.y,current_accel.z);

  //4.getting processed value from puara
  puara_gestures::Coord3D shake_vector = impl.current_value();

  //5. calculating the desired float value

  float shake_magnitude = std::sqrt((shake_vector.x * shake_vector.x) +
      (shake_vector.y * shake_vector.y) +
      (shake_vector.z * shake_vector.z));

  //6 setting the output port value
  outputs.output.value = shake_magnitude;

}
}
