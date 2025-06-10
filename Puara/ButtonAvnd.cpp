#include "ButtonAvnd.hpp"

namespace puara_gestures::objects
{

void ButtonAvnd::operator()()
{
  const int current_button_state = inputs.button_input;

  this->impl.threshold = inputs.threshold_param;
  this->impl.countInterval = inputs.count_interval_param;
  this->impl.holdInterval = inputs.hold_interval_param;

  this->impl.update(current_button_state);

  outputs.is_pressed_output = this->impl.press;

  outputs.tap_event_output = this->impl.tap;
  outputs.double_tap_event_output = this->impl.doubleTap;
  outputs.triple_tap_event_output = this->impl.tripleTap;

  outputs.is_held_output = this->impl.hold;
  outputs.press_duration_output = this->impl.pressTime;
  outputs.internal_tap_count_output = this->impl.count;
}

}
