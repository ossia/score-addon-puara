#pragma once

#include "3rdparty/extras/PeakDetector.h"

#include <halp/audio.hpp>
#include <halp/controls.hpp>
#include <halp/meta.hpp>
#include <puara/gestures.h>

#include <halp/controls.enums.hpp>

namespace puara_gestures::objects
{
class PeakDetection
{
public:
  halp_meta(name, "Peak detection")
  halp_meta(category, "Data analysis")
  halp_meta(c_name, "Peak_detection")
  halp_meta(author, "Luana Belinsky")
  halp_meta(uuid, "031bc209-5097-44ac-99c1-ade065a0c02d")

  struct ins
  {
    halp::val_port<"Peak_detection_signal", float> peakDetection_signal;
    struct peak_mode
    {
      halp__enum_combobox(
          "Mode", PEAK_MAX, PEAK_MAX, PEAK_MIN, PEAK_RISING, PEAK_FALLING)
    } mode;
  } inputs;

  struct
  {
    halp::val_port<"Peak", float> peak;
  } outputs;

  halp::setup setup;
  void prepare(halp::setup info);

  using tick = halp::tick;
  void operator()(halp::tick t);
  bool toggle = 0;
};
}
