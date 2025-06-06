#include "ButtonAvnd.hpp"
#include <puara/utils.h>

namespace puara_gestures::objects
{

void ButtonAvnd::operator()()
{
  // 1. Getting the current button state from the input port
  const int current_button_state = inputs.button_input.value;

  // 2. Updating configurable parameters of the underlying puara_gestures::Button object

  unsigned int desired_threshold = static_cast<unsigned int>(inputs.threshold_param.value);
  if (this->impl.threshold != desired_threshold) {
    this->impl.threshold = desired_threshold;
  }

  unsigned int desired_count_interval = static_cast<unsigned int>(inputs.count_interval_param.value);
  if (this->impl.countInterval != desired_count_interval) {
    this->impl.countInterval = desired_count_interval;
  }

  unsigned int desired_hold_interval = static_cast<unsigned int>(inputs.hold_interval_param.value);
  if (this->impl.holdInterval != desired_hold_interval) {
    this->impl.holdInterval = desired_hold_interval;
  }


  // 3. Calling the update method of the puara_gestures::Button implementation.
  this->impl.update(current_button_state);

  // 4. Updating Avendish output ports with the results from the puara_gestures::Button object.
  outputs.is_pressed_output.value = this->impl.press;
  outputs.tap_event_output.value = (this->impl.tap == 1);
  outputs.double_tap_event_output.value = (this->impl.doubleTap == 1);
  outputs.triple_tap_event_output.value = (this->impl.tripleTap == 1);

  outputs.is_held_output.value = this->impl.hold;
  outputs.press_duration_output.value = static_cast<float>(this->impl.pressTime);
}

}
