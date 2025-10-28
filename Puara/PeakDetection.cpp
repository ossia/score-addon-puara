#include "PeakDetection.hpp"

namespace puara_gestures::objects
{

void PeakDetection::update_params(float trig, float reload, float fallback)
{
  for (auto& d : det)
  {
    d.triggerThreshold(trig);
    d.reloadThreshold(reload);
    d.fallbackTolerance(fallback);
  }
}

void PeakDetection::prepare(halp::setup info)
{
  setup = info;

  // Initialize watchers to current UI state
  trig_watch.last     = inputs.trig_thresh;  trig_watch.first = false;
  reload_watch.last   = inputs.reload_thresh;reload_watch.first = false;
  fallback_watch.last = inputs.fallback_tol; fallback_watch.first = false;

  update_params(inputs.trig_thresh, inputs.reload_thresh, inputs.fallback_tol);
}

void PeakDetection::operator()(halp::tick /*t*/)
{
  // Update params when changed 
  bool params_changed = false;

  if (trig_watch.changed(inputs.trig_thresh))   params_changed = true;
  if (reload_watch.changed(inputs.reload_thresh)) params_changed = true;
  if (fallback_watch.changed(inputs.fallback_tol)) params_changed = true;

  if (params_changed)
    update_params(inputs.trig_thresh, inputs.reload_thresh, inputs.fallback_tol);

  const float v = inputs.peakDetection_signal;

  // Compute each detector’s output
  const bool rising  = det[PEAK_RISING ].put(v);
  const bool falling = det[PEAK_FALLING].put(v);
  const bool pmax    = det[PEAK_MAX    ].put(v);
  const bool pmin    = det[PEAK_MIN    ].put(v);

  // Output
  outputs.peak_rising  = rising;
  outputs.peak_falling = falling;
  outputs.peak_max     = pmax;
  outputs.peak_min     = pmin;
}

} // namespace
