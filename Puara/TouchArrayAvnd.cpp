#include "TouchArrayAvnd.hpp"

namespace puara_gestures::objects
{

void TouchArrayAvnd::operator()()
{
  // 1. Getting input touch array and its size
  // Useing .value here as std::vector doesn't have an implicit conversion from halp::val_port
  std::vector<int>& current_touch_array_data = inputs.touch_array_input.value;
  const int touch_size = static_cast<int>(current_touch_array_data.size());

  if(touch_size == 0 || touch_size < 2 * TAGD_TOUCH_SIZE_EDGE)
  {
    outputs.total_touch_avg.value = 0.0f;
    outputs.top_touch_avg.value = 0.0f;
    outputs.middle_touch_avg.value = 0.0f;
    outputs.bottom_touch_avg.value = 0.0f;
    outputs.total_brush.value = 0.0f;
    outputs.total_rub.value = 0.0f;
    return;
  }
  impl.update(current_touch_array_data.data(), touch_size);

  outputs.total_touch_avg.value = impl.totalTouchAverage;
  outputs.top_touch_avg.value = impl.topTouchAverage;
  outputs.middle_touch_avg.value = impl.middleTouchAverage;
  outputs.bottom_touch_avg.value = impl.bottomTouchAverage;
  outputs.total_brush.value = impl.totalBrush;
  outputs.total_rub.value = impl.totalRub;
}

}
