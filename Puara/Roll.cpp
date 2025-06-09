#include "Roll.hpp"

namespace puara_gestures::objects
{
void Roll::operator()(halp::tick t)
{
  if(setup.rate <= 0.0 || t.frames <= 0)
  {

    return;
  }

  const double period_s = static_cast<double>(t.frames) / setup.rate;
  const bool unwrap_enabled = inputs.enable_unwrap;
  const bool smooth_enabled = inputs.enable_smooth;
  const bool wrap_enabled = inputs.enable_wrap;

  // 4. Calling roll calculation.
  double current_roll_rad = impl.roll(inputs.accel, inputs.gyro, inputs.mag, period_s);

  if(unwrap_enabled)
  {
    current_roll_rad = impl.unwrap(current_roll_rad);
  }
  if(smooth_enabled)
  {
    current_roll_rad = impl.smooth(current_roll_rad);
  }
  if(wrap_enabled)
  {
    current_roll_rad = impl.wrap(current_roll_rad);
  }

  outputs.output.value = current_roll_rad;
}
}
