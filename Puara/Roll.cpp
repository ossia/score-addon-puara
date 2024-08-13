#include "Roll.hpp"
namespace puara_gestures::objects
{
void Roll::operator()(halp::tick t)
{
  outputs.output = impl.roll(inputs.accel, inputs.gyro, inputs.mag, 0.1);
}
}
