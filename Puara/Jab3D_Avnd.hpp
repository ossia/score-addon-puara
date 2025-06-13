#pragma once

#include <halp/audio.hpp>
#include <halp/controls.hpp>
#include <halp/meta.hpp>
#include <puara/descriptors/jab.h>

namespace puara_gestures::objects
{
class Jab
{
public:
  halp_meta(name, "Jab 3D")
  halp_meta(category, "Gestures")
  halp_meta(c_name, "puara_jab_3d")
  halp_meta(
      description,
      "Detects 3D jab gestures from accelerometer data, outputting per-axis jab "
      "intensity. Threshold is configurable.")
  halp_meta(manual_url, "https://github.com/Puara/puara-gestures/")
  halp_meta(uuid, "70cf4250-d66e-4a78-824f-4b51fb3d0075")

  struct ins
  {
    halp::val_port<"Acceleration 3D", puara_gestures::Coord3D> accel_3D;

    halp::knob_f32<"Threshold", halp::range{0.0, 10.0, 5.0}> jab_threshold;

  } inputs;

  struct outputs
  {
    halp::val_port<"Output 3D", puara_gestures::Coord3D> output_3d;
  } outputs;

  void operator()();

  puara_gestures::Jab3D impl;
};

}
