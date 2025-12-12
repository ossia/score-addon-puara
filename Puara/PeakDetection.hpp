#pragma once

#include "3rdparty/extras/PeakDetector.h"

#include <array>
#include <halp/audio.hpp>
#include <halp/controls.hpp>
#include <halp/meta.hpp>
#include <puara/gestures.h>
#include "halp_utils.hpp"

namespace puara_gestures::objects
{
// peak detector
class PeakDetection
{
public:
  halp_meta(name, "Peak detection")
  halp_meta(category, "Analysis/Data")
  halp_meta(c_name, "Peak_detection")
  halp_meta(author, "Luana Belinsky (adapted from Sofian Audry’s Plaquette)")
  halp_meta(
      description,
      "Detects peaks in an incoming normalized signal. "
      "Four detectors analyze the input to output triggers for rising, falling, "
      "maximum, and minimum peaks. \n"
      "Input is expected in the range [0, 1]. \n"
      "Use normalization / scaling objects upstream if needed."
  )
  halp_meta(manual_url, "https://plaquette.org/PeakDetector.html")
  halp_meta(uuid, "031bc209-5097-44ac-99c1-ade065a0c02d")

  struct
  {
    halp::data_port<
        "Peak detection signal",
        "Input signal. Float between 0 and 1",
        float>
        peakDetection_signal;

    halp::knob_f32<
        "Trigger threshold",
        halp::range{0.0f, 1.0f, 0.5f}>
        trig_thresh; // Level that starts detection

    halp::knob_f32<
        "Reload threshold",
        halp::range{0.0f, 1.0f, 0.35f}>
        reload_thresh; // Level below which the detector resets

    halp::knob_f32<
        "Fallback tolerance",
        halp::range{0.0, 1.0, 0.10f}>
        fallback_tol; // Drop percentage between trigger and apex
  } inputs;

  struct
  {
    halp::data_port<
        "Peak max",
        "Boolean. True when a local maximum (apex) is detected.",
        bool>
        peak_max;

    halp::data_port<
        "Peak min",
        "Boolean. True when a local minimum (valley) is detected.",
        bool>
        peak_min;

    halp::data_port<
        "Peak rising",
        "Boolean. True when signal crosses upward above the trigger threshold.",
        bool>
        peak_rising;

    halp::data_port<
        "Peak falling",
        "Boolean. True when signal crosses downward below the trigger threshold.",
        bool>
        peak_falling;
  } outputs;

  halp::setup setup;
  void prepare(halp::setup info);

  using tick = halp::tick;
  void operator()(halp::tick t);

private:
  // Index directly by the enum: 0=MAX, 1=MIN, 2=RISING, 3=FALLING
  std::array<PeakDetector, 4> det{
      PeakDetector{0.5f, PEAK_MAX},
      PeakDetector{0.5f, PEAK_MIN},
      PeakDetector{0.5f, PEAK_RISING},
      PeakDetector{0.5f, PEAK_FALLING}};

  halp::ParameterWatcher<float> trig_watch;
  halp::ParameterWatcher<float> reload_watch;
  halp::ParameterWatcher<float> fallback_watch;
};

} // namespace puara_gestures::objects
