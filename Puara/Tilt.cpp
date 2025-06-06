#include "Tilt.hpp"

namespace puara_gestures::objects
{
void Tilt::operator()(halp::tick t)
{

  if(setup.rate <= 0.0 || t.frames <= 0)
  {
    return;
  }

  // Casting t.frames to double ensures floating-point division.
  const double period_s = static_cast<double>(t.frames) / setup.rate;

  // Calling puara_gestures::Tilt::tilt method with direct inputs.
  const double tilt_value_rad
      = impl.tilt(inputs.accel.value, inputs.gyro.value, inputs.mag.value, period_s);

  // Setting the output port value.
  outputs.output.value = tilt_value_rad;
}
}
