#pragma once

#include <halp/controls.hpp>
#include <halp/meta.hpp>
#include <puara/descriptors/jab.h>

namespace puara_gestures::objects
{

class Jab1D_Avnd
{
public:
  halp_meta(name, "Jab 1D")
  halp_meta(category, "Gestures")
  halp_meta(c_name, "puara_jab_1d_avnd")
  halp_meta(description, "Detects jab gestures from a single axis of acceleration data.")
  halp_meta(manual_url, "https://github.com/Puara/puara-gestures/")
  halp_meta(uuid, "9444c79a-3e32-4c76-a755-019096fc4476")

  struct ins
  {
    halp::val_port<"Acceleration 1D", float> acceleration_1d{0.0f};

    halp::knob_f32<"Threshold", halp::range{0.0, 50.0, 5.0}> threshold_param;
  } inputs;

  struct outputs
  {
    halp::val_port<"Output 1D", float> output_1d{0.0f};
  } outputs;

  void operator()();

  puara_gestures::Jab impl;
};

}
