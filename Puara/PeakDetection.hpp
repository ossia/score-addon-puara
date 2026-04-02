#pragma once

#include "3rdparty/extras/PeakDetector.h"

#include <algorithm>
#include <cfloat>
#include <halp/audio.hpp>
#include <halp/controls.hpp>
#include <halp/meta.hpp>
#include <puara/gestures.h>

#include "halp_utils.hpp"

namespace puara_gestures::objects
{

class PeakDetection
{
public:

  halp_meta(name, "Peak detection")
  halp_meta(category, "Analysis/Data")
  halp_meta(c_name, "Peak_detection")
  halp_meta(author, "Luana Belinsky")
  halp_meta(description,
    "Detects trigger crossings and peaks from a normalized detection signal, "
    "and measures amplitude from a separate raw signal. "
    "Peak-to-peak, max above threshold, and min below threshold use raw "
    "values paired with confirmed detect peaks. "
    "Raw peak-to-peak is computed independently from raw turning points.")
  halp_meta(uuid, "031bc209-5097-44ac-99c1-ade065a0c02d")

  enum class P2PUpdateMode { On_max, On_min, On_both };

  struct
  {
    halp::data_port<
      "Detection signal",
      "Normalized signal used for trigger crossings and peak detection",
      float> detect_signal;

    halp::data_port<
      "Raw signal",
      "Original signal used for amplitude measurement",
      float> raw_signal;

    halp::knob_f32<
      "Trigger threshold",
      halp::range{0.f, 1.f, 0.5f}> trig_thresh;

    halp::knob_f32<
      "Reload threshold",
      halp::range{0.f, 1.f, 0.35f}> reload_thresh;

    halp::knob_f32<
      "Fallback tolerance",
      halp::range{0.f, 1.f, 0.1f}> fallback_tol;

    halp::enum_t<P2PUpdateMode, "P2P update">
      p2p_update_mode{P2PUpdateMode::On_max};

  } inputs;


  struct
  {
    halp::data_port<"Peak max",     "Pulse on local maximum", bool> peak_max;
    halp::data_port<"Peak min",     "Pulse on local minimum", bool> peak_min;

    halp::data_port<"Peak rising",  "Pulse on upward trigger-threshold crossing",  bool> peak_rising;
    halp::data_port<"Peak falling", "Pulse on downward trigger-threshold crossing", bool> peak_falling;

    halp::data_port<"Peak-to-peak",        "Held detect-locked peak-to-peak amplitude", float> peak_to_peak;
    halp::data_port<"Raw peak-to-peak",    "Held raw peak-to-peak amplitude from raw turning points", float> raw_peak_to_peak;
    halp::data_port<"Max above threshold", "Held raw amplitude above the rising trigger baseline", float> max_above_threshold;
    halp::data_port<"Min below threshold", "Held raw amplitude below the falling trigger baseline", float> min_below_threshold;
  } outputs;


  halp::setup setup;

  void prepare(halp::setup info);

  using tick = halp::tick;
  void operator()(halp::tick);


private:

  // --------------------------------------------------
  // Peak detectors
  // --------------------------------------------------

  PeakDetector det_rising_{0.5f, PEAK_RISING};
  PeakDetector det_falling_{0.5f, PEAK_FALLING};
  PeakDetector det_max_{0.5f, PEAK_MAX};
  PeakDetector det_min_{0.5f, PEAK_MIN};


  // --------------------------------------------------
  // Previous sample memory
  // --------------------------------------------------

  bool  has_prev_ = false;

  float prev_detect_ = 0.f;
  float prev_raw_    = 0.f;


  // --------------------------------------------------
  // Raw baselines interpolated at the detect trigger crossings
  // --------------------------------------------------

  float raw_at_rising_  = 0.f;
  float raw_at_falling_ = 0.f;


  // --------------------------------------------------
  // Detect-based candidate tracking
  // --------------------------------------------------

  bool track_max_ = false;
  bool track_min_ = false;

  float best_detect_max_ = -FLT_MAX;
  float best_detect_min_ =  FLT_MAX;

  float raw_at_max_candidate_ = 0.f;
  float raw_at_min_candidate_ = 0.f;

  float raw_at_max_ = 0.f;
  float raw_at_min_ = 0.f;


  // Raw turning-point tracking for raw peak-to-peak
  // --------------------------------------------------

  int   prev_raw_slope_ = 0;
  bool  have_raw_max_   = false;
  bool  have_raw_min_   = false;
  float last_raw_max_   = 0.f;
  float last_raw_min_   = 0.f;


  // --------------------------------------------------
  // Held amplitude outputs
  // --------------------------------------------------

  float last_p2p_ = 0.f;
  float last_raw_p2p_ = 0.f;
  float last_mat_ = 0.f;
  float last_mbt_ = 0.f;
};

}
