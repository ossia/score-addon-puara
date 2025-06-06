#pragma once
#include <halp/audio.hpp>
#include <halp/controls.hpp>
#include <halp/meta.hpp>
#include <puara/descriptors/button.h>
#include <puara/utils.h>
#include <halp/controls.basic.hpp>

namespace puara_gestures::objects
{

class ButtonAvnd{
public:
  halp_meta(name,"Button Processor")
  halp_meta(cateogry,"Utilities/Puara")
  halp_meta(c_name,"puara_button_avnd")
  halp_meta(uuid,"e5cbc939-0df0-4256-bcfa-38ec67fb6fcd")


  struct ins{
    halp::val_port<"Input",int>button_input{0};
    halp::knob_f32<"Threshold",halp::range{0.0,10.0,1.0}>threshold_param;
    halp::knob_f32<"Count Interval (ms)",halp::range{50.0,1000.0,200.0}>count_interval_param;
    halp::knob_f32<"Hold Interval (ms)",halp::range{100.0,10000.0,5000.0}>hold_interval_param;


  } inputs;

  struct outputs{

    //extracted features
    halp::val_port<"Is Pressed",bool> is_pressed_output{false};
    halp::val_port<"Tap Event",bool> tap_event_output{false};
    halp::val_port<"Double Tap Event", bool> double_tap_event_output{false};
    halp::val_port<"Triple Tap Event", bool> triple_tap_event_output{false};
    halp::val_port<"Is Held", bool> is_held_output{false};
    halp::val_port<"Last Press Duration (ms)", float> press_duration_output{0.0f};
  } outputs;


  void operator()();

  puara_gestures::Button impl;

  ButtonAvnd() = default;


};



}
