#include "Roll.hpp"

namespace puara_gestures::objects
{
void Roll::operator()(halp::tick t)
{
  if(setup.rate <= 0.0 || t.frames <= 0)
  {

    return;
  }

  // 2. Calculating the processing period in seconds.
  const double period_s = static_cast<double>(t.frames) / setup.rate;

  const bool unwrap_enabled = inputs.enable_unwrap.value;
  const bool smooth_enabled = inputs.enable_smooth.value;
  const bool wrap_enabled = inputs.enable_wrap.value;

  // 4. Calling roll calculation.
  double current_roll_rad
      = impl.roll(inputs.accel.value, inputs.gyro.value, inputs.mag.value, period_s);

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
