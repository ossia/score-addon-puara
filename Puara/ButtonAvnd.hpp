#pragma once
#include <halp/audio.hpp>
#include <halp/controls.hpp>
#include <halp/meta.hpp>
#include <puara/descriptors/button.h>

namespace puara_gestures::objects
{
class ButtonAvnd
{
public:
  halp_meta(name, "Button Processor")
  halp_meta(category, "Gestures")
  halp_meta(c_name, "puara_button_avnd")
  halp_meta(
      description,
      "Extracts features like press, hold, tap, double-tap, and triple-tap from a "
      "button input.")
  halp_meta(manual_url, "https://github.com/Puara/puara-gestures/")
  halp_meta(uuid, "e5cbc939-0df0-4256-bcfa-38ec67fb6fcd")

  struct
  {
    halp::val_port<"Input", int> button_input{0};
    halp::knob_f32<"Threshold", halp::range{0.0, 10.0, 1.0}> threshold_param;
    halp::knob_f32<"Count Interval (ms)", halp::range{50.0, 1000.0, 200.0}>
        count_interval_param;
    halp::knob_f32<"Hold Interval (ms)", halp::range{100.0, 10000.0, 5000.0}>
        hold_interval_param;
  } inputs;

  struct
  {
    halp::val_port<"Is Pressed", bool> is_pressed_output{false};
    halp::val_port<"Tap Event", int> tap_event_output{0};
    halp::val_port<"Double Tap Event", int> double_tap_event_output{0};
    halp::val_port<"Triple Tap Event", int> triple_tap_event_output{0};

    halp::val_port<"Is Held", bool> is_held_output{false};
    halp::val_port<"Last Press Duration (ms)", float> press_duration_output{0.0f};
    halp::val_port<"Internal Tap Count", int> internal_tap_count_output{0};
  } outputs;

  void operator()();
  puara_gestures::Button impl;
};
}
