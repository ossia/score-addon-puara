/*
 * PeakDetector.cpp
 *
 * Adapté de : 
 * Plaquette (c) 2022 Sofian Audry        :: info(@)sofianaudry(.)com
 *
 * Adaptation par Luana Belinsky 2025
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
