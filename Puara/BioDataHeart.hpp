#pragma once

#include "3rdparty/BioData/src/Heart.h"

#include <halp/audio.hpp>
#include <halp/controls.hpp>
#include <halp/meta.hpp>
#include <puara/gestures.h>

namespace puara_gestures::objects
{
class BioData_Heart
{
public:
  halp_meta(name, "Heart")
  halp_meta(category, "BioData")
  halp_meta(c_name, "BioData_Heart")
  halp_meta(
      author,
      "BioData authors, Erin Gee, Martin Peach, Thomas Ouellet Fredericks, Sofian "
      "Audry, Luana Belinsky")
  halp_meta(manual_url, "https://github.com/eringee/BioData")
  halp_meta(uuid, "53b50cf0-4f0f-499c-91e9-a5fb34ec8a15")

  struct ins
  {
    halp::val_port<"Heart_signal", float> heart_signal;
  } inputs;

  struct
  {
    halp::val_port<"Heart_raw", float> heart_raw;
    halp::val_port<"Heart_normalized", float> heart_normalized;
    halp::val_port<"Heart_beatDetected", bool> heart_beat;
    halp::val_port<"Heart_bpm", float> heart_bpm;
    halp::val_port<"Heart_bpmChange", float> heart_bpmChange;
    halp::val_port<"Heart_amplitudeChange", float> heart_amplitudeChange;
  } outputs;

  halp::setup setup;
  void prepare(halp::setup info) { setup = info; }

  using tick = halp::tick;
  void operator()(halp::tick t);

  Heart heart;
};
}
