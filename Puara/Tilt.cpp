#include "Tilt.hpp"
namespace puara_gestures::objects
{
void Tilt::operator()(halp::tick t)
{
  outputs.output = impl.tilt(inputs.accel, inputs.gyro, inputs.mag, 0.1);
}
}
