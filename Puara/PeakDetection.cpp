#include "PeakDetection.hpp"

#include <algorithm>
#include <cfloat>

namespace puara_gestures::objects
{
// Estimate the raw value at the detect-threshold crossing between two samples.
static inline float interp_raw_at(float d0, float d1, float r0, float r1, float thr)
{
  const float denom = (d1 - d0);
  if (denom == 0.f) return r1;
  const float t = std::clamp((thr - d0) / denom, 0.f, 1.f);
  return r0 + (r1 - r0) * t;
}

void PeakDetection::prepare(halp::setup info)
{
  setup = info;

  outputs.peak_max = outputs.peak_min = outputs.peak_rising = outputs.peak_falling = false;

  outputs.peak_to_peak        = last_p2p_;
  outputs.raw_peak_to_peak    = last_raw_p2p_;
  outputs.max_above_threshold = last_mat_;
  outputs.min_below_threshold = last_mbt_;

  has_prev_ = false;
  prev_detect_ = 0.f;
  prev_raw_    = 0.f;

  raw_at_rising_  = 0.f;
  raw_at_falling_ = 0.f;

  track_max_ = track_min_ = false;
  best_detect_max_ = -FLT_MAX;
  best_detect_min_ = +FLT_MAX;
  raw_at_max_candidate_ = 0.f;
  raw_at_min_candidate_ = 0.f;

  raw_at_max_ = 0.f;
  raw_at_min_ = 0.f;

  prev_raw_slope_ = 0;
  have_raw_max_   = false;
  have_raw_min_   = false;
  last_raw_max_   = 0.f;
  last_raw_min_   = 0.f;
}

void PeakDetection::operator()(halp::tick)
{
  const float d   = inputs.detect_signal;
  const float raw = inputs.raw_signal;

  // Parameters
  const float trig     = inputs.trig_thresh;
  const float reload   = inputs.reload_thresh;
  const float fallback = inputs.fallback_tol;

  // Keep detector thresholds in sync with the UI.
  for (auto* det : {&det_rising_, &det_falling_, &det_max_, &det_min_})
  {
    det->triggerThreshold(trig);
    det->reloadThreshold(reload);
    det->fallbackTolerance(fallback);
  }

  const P2PUpdateMode p2p_mode = inputs.p2p_update_mode;

  // Events derived from the detection signal.
  // PEAK_FALLING fires on the downward crossing of triggerThreshold.
  // reloadThreshold only re-arms the detector.
  const bool rising_gate  = det_rising_.put(d);   // crosses UP triggerThreshold
  const bool falling_gate = det_falling_.put(d);  // crosses DOWN triggerThreshold
  const bool pmax_gate    = det_max_.put(d);      // confirmed detect max
  const bool pmin_gate    = det_min_.put(d);      // confirmed detect min

  outputs.peak_rising  = rising_gate;
  outputs.peak_falling = falling_gate;
  outputs.peak_max     = pmax_gate;
  outputs.peak_min     = pmin_gate;

  // First sample: initialize history and keep the held outputs.
  if (!has_prev_)
  {
    outputs.peak_rising = outputs.peak_falling = outputs.peak_max = outputs.peak_min = false;
    prev_detect_ = d;
    prev_raw_    = raw;
    has_prev_    = true;
    return;
  }

  // Keep the raw value paired with the strongest detect sample
  // seen since the relevant trigger crossing. The >= / <= tests keep the
  // latest sample on flat plateaus.
  if (track_max_ && d >= best_detect_max_) { best_detect_max_ = d; raw_at_max_candidate_ = raw; }
  if (track_min_ && d <= best_detect_min_) { best_detect_min_ = d; raw_at_min_candidate_ = raw; }

  // Raw peak-to-peak uses turning points detected directly on the raw signal.
  const float raw_delta = raw - prev_raw_;
  const int raw_slope = (raw_delta > 0.f) ? 1 : (raw_delta < 0.f ? -1 : prev_raw_slope_);
  if (prev_raw_slope_ > 0 && raw_slope < 0)
  {
    last_raw_max_ = prev_raw_;
    have_raw_max_ = true;
    if (have_raw_min_)
      last_raw_p2p_ = std::max(0.f, last_raw_max_ - last_raw_min_);
  }
  else if (prev_raw_slope_ < 0 && raw_slope > 0)
  {
    last_raw_min_ = prev_raw_;
    have_raw_min_ = true;
    if (have_raw_max_)
      last_raw_p2p_ = std::max(0.f, last_raw_max_ - last_raw_min_);
  }
  prev_raw_slope_ = raw_slope;

  // Rising trigger crossing: latch the raw baseline at the upward trigger crossing.
  if (rising_gate)
  {
    raw_at_rising_ = interp_raw_at(prev_detect_, d, prev_raw_, raw, trig);

    // Start tracking the raw sample associated with the next detect max.
    track_max_ = true;
    best_detect_max_ = -FLT_MAX;
    raw_at_max_candidate_ = raw;
  }

  // Falling trigger crossing: latch the raw baseline at the downward trigger crossing.
  if (falling_gate)
  {
    raw_at_falling_ = interp_raw_at(prev_detect_, d, prev_raw_, raw, trig);

    // Start tracking the raw sample associated with the next detect min.
    track_min_ = true;
    best_detect_min_ = +FLT_MAX;
    raw_at_min_candidate_ = raw;
  }

  // Detect-based amplitudes update only when the detect maxima/minima are confirmed.
  {
    if (pmax_gate)
    {
      raw_at_max_ = track_max_ ? raw_at_max_candidate_ : raw;
      track_max_  = false;

      last_mat_ = std::max(0.f, raw_at_max_ - raw_at_rising_);

      if (p2p_mode == P2PUpdateMode::On_max || p2p_mode == P2PUpdateMode::On_both)
        last_p2p_ = std::max(0.f, raw_at_max_ - raw_at_min_);
    }

    if (pmin_gate)
    {
      raw_at_min_ = track_min_ ? raw_at_min_candidate_ : raw;
      track_min_  = false;

      // Min-below is measured from the falling trigger baseline down to the detect min.
      last_mbt_ = std::max(0.f, raw_at_falling_ - raw_at_min_);

      if (p2p_mode == P2PUpdateMode::On_min || p2p_mode == P2PUpdateMode::On_both)
        last_p2p_ = std::max(0.f, raw_at_max_ - raw_at_min_);
    }
  }

  // Publish held values every tick.
  outputs.peak_to_peak        = last_p2p_;
  outputs.raw_peak_to_peak    = last_raw_p2p_;
  outputs.max_above_threshold = last_mat_;
  outputs.min_below_threshold = last_mbt_;

  // Update history for interpolation on the next tick.
  prev_detect_ = d;
  prev_raw_    = raw;
  has_prev_    = true;
}

} // namespace puara_gestures::objects
