#pragma once

#include "3rdparty/extras/PeakDetector.h"

#include <array>
#include <halp/audio.hpp>
#include <halp/controls.hpp>
#include <halp/meta.hpp>
#include <halp/controls.enums.hpp>
#include <puara/gestures.h>

// data_port struct to include descriptions
namespace halp {
template <static_string lit, static_string desc, typename T>
struct data_port : val_port<lit, T>
{
  using base_t = val_port<lit, T>;
  using base_t::operator=;   

  static clang_buggy_consteval auto description() { return std::string_view{desc.value}; }
};
} // namespace halp

namespace puara_gestures::objects
{

// parameter watcher to trigger when a parameter is changed
template<typename T> 
struct ParameterWatcher { 
  T last{}; 
  bool first = true; 
  bool changed(const T& current) { 
    if (first || current != last) { 
      last = current; first = false; return true; 
    } return false; 
  } 
};

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
      "Detects peaks (maxima or minima) in an incoming normalized signal. "
      "Four detectors analyze the input to output triggers for rising, falling, "
      "maximum, and minimum peaks. \n"
      "Tip: Normalize (0 to 1) and smooth your signal before feeding it to the peak detector. \n"
  )
  halp_meta(manual_url, "https://plaquette.org/PeakDetector.html")
  halp_meta(uuid, "031bc209-5097-44ac-99c1-ade065a0c02d")

  struct
  {
    halp::data_port<
        "Peak detection signal",
        "Input signal (float, ideally normalized 0 to 1).",
        float>
        peakDetection_signal;

    halp::knob_f32<
        "Trigger threshold",
        halp::range{0.0, 1.0, 0.5}>
        trig_thresh; // Level that starts detection

    halp::knob_f32<
        "Reload threshold",
        halp::range{0.0, 1.0, 0.35}>
        reload_thresh; // Level below which the detector resets

    halp::knob_f32<
        "Fallback tolerance",
        halp::range{0.0, 1.0, 0.10}>
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

  void update_params(float trig, float reload, float fallback);

  ParameterWatcher<float> trig_watch;
  ParameterWatcher<float> reload_watch;
  ParameterWatcher<float> fallback_watch;

  
};

} // namespace puara_gestures::objects
