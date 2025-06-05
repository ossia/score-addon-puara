#include "Tilt.hpp"

namespace puara_gestures::objects
{
void Tilt::operator()(halp::tick t)
{
  //1. calcutating the processing period --> duraring of this->tick in sec
  double period_s = 0.0 ;
  if(setup.rate>0 && t.frames>0) {//as learnt setup.rate is samples per second
    period_s = static_cast<double>(t.frames/setup.rate);
  }else{}

  //2, getting input data
  const puara_gestures::Coord3D &current_accel = inputs.accel.value;
  const puara_gestures::Coord3D &current_gyro = inputs.gyro.value;
  const puara_gestures::Coord3D &current_mag = inputs.mag.value;

  //3. calling puara tilt impl process method
  double tilt_value_rad = impl.tilt(current_accel,current_gyro,current_mag,period_s);

  //4 setting the output port value
  // also to note the output value is float and the tilt_value_rat is a double
  outputs.output.value = static_cast<float>(tilt_value_rad);

}
}
