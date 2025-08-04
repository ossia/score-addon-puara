#pragma once

#include <halp/controls.hpp>
#include <halp/meta.hpp>
#include <puara/descriptors/jab.h>
#include <puara/structs.h>

namespace puara_gestures::objects
{

class Jab2D_Avnd
{
public:
  halp_meta(name, "Jab 2D")
  halp_meta(category, "Analysis/Gestures")
  halp_meta(c_name, "puara_jab_2d_avnd")
  halp_meta(
      description, "Detects jab gestures from two axes of acceleration data (X and Y).")
  halp_meta(manual_url, "https://github.com/Puara/puara-gestures")
  halp_meta(uuid, "f006bc9b-5148-469f-af35-d3c4360149bd")

  struct
  {
    halp::val_port<"Acceleration 2D", puara_gestures::Coord2D> acceleration_2d;

    halp::knob_f32<"Threshold", halp::range{0.0, 50.0, 5.0}> threshold_param;
  } inputs;

  struct
  {
    halp::val_port<"Output 2D", puara_gestures::Coord2D> output_2d;
  } outputs;

  void operator()();

  puara_gestures::Jab2D impl;
};

}
