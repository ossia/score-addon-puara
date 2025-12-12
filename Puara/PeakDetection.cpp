#include "PeakDetection.hpp"

namespace puara_gestures::objects
{
void PeakDetection::prepare(halp::setup info)
{
  setup = info;

  // Initialize watchers to current UI state
  trig_watch.last     = inputs.trig_thresh;  trig_watch.first = false;
  reload_watch.last   = inputs.reload_thresh;reload_watch.first = false;
  fallback_watch.last = inputs.fallback_tol; fallback_watch.first = false;
}

void PeakDetection::operator()(halp::tick /*t*/)
{
  const bool trig_changed     = trig_watch.changed(inputs.trig_thresh);
  const bool reload_changed   = reload_watch.changed(inputs.reload_thresh);
  const bool fallback_changed = fallback_watch.changed(inputs.fallback_tol);

  if (trig_changed || reload_changed || fallback_changed)
  {
    // Update only what actually changed, across all detectors
    for (auto& d : det)
    {
      if (trig_changed)
        d.triggerThreshold(inputs.trig_thresh);

      if (reload_changed)
        d.reloadThreshold(inputs.reload_thresh);

      if (fallback_changed)
        d.fallbackTolerance(inputs.fallback_tol);
    }
  }
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
