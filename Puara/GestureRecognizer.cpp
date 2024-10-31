#include "GestureRecognizer.hpp"
namespace puara_gestures::objects
{
void GestureRecognizer::operator()(halp::tick t)
{
  const double period = t.frames / setup.rate;

  auto [ax, ay, az] = inputs.accel.value;
  jab.update(ax, ay, az);
  shake.update(ax, ay, az);

  outputs.jab = jab.current_value();
  outputs.shake = shake.current_value();
  outputs.tilt = tilt.tilt(inputs.accel, inputs.gyro, inputs.mag, period);
  outputs.roll = roll.roll(inputs.accel, inputs.gyro, inputs.mag, period);
}
}
