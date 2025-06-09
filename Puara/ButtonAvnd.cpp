#include "ButtonAvnd.hpp"

namespace puara_gestures::objects
{

void ButtonAvnd::operator()()
{
  //input
  const int current_button_state = inputs.button_input;
  //Updating configurable parameters
  this->impl.threshold = inputs.threshold_param;
  this->impl.countInterval = inputs.count_interval_param;
  this->impl.holdInterval = inputs.hold_interval_param;
  //calling update methords
  this->impl.update(current_button_state);
  //updating output ports
  outputs.is_pressed_output = this->impl.press;
  outputs.tap_event_output = (this->impl.tap == 1);
  outputs.double_tap_event_output = (this->impl.doubleTap == 1);
  outputs.triple_tap_event_output = (this->impl.tripleTap == 1);
  outputs.is_held_output = this->impl.hold;
  outputs.press_duration_output = this->impl.pressTime;
}

}
