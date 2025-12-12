#include "Scaler.hpp"

#include <limits>
#include <cmath>
#include <algorithm>

namespace puara_gestures::objects
{

void Scaler::prepare(halp::setup info)
{
  setup = info;

  // Initialize both scalers with default config.
  minmax   = MinMaxScaler();   // infinite window by default
  quantile = QuantileScaler(); // infinite window by default

  // Initial time window setup from UI.
  const bool  infinite = inputs.infinite_time_window;
  const float tw       = inputs.time_window;

  const double tw_seconds = infinite ? 0.0
                                     : static_cast<double>(tw);

  minmax.timeWindow(tw_seconds);
  quantile.timeWindow(tw_seconds);

  // Initial span for quantile mode.
  quantile.span(inputs.span);

  // Initial mode from UI.
  Mode mode_value = inputs.mode.value; 
  current_mode    = mode_value;

  // Prime watchers with initial values
  mode_watcher.changed(mode_value);
  infinite_watcher.changed(infinite);
  time_window_watcher.changed(tw);
  span_watcher.changed(inputs.span);
}

void Scaler::operator()(halp::tick t)
{
  // --- live param updates (only when changed) ---
  Mode mode_value = static_cast<Mode>(inputs.mode);
  const bool mode_changed = mode_watcher.changed(mode_value);

  const bool  infinite = inputs.infinite_time_window;
  const float tw       = inputs.time_window;
  const bool  infinite_changed = infinite_watcher.changed(infinite);
  const bool  time_changed = time_window_watcher.changed(tw);

  const float span = inputs.span;
  const bool  span_changed = span_watcher.changed(span);

  // --- handle mode change ---
  if(mode_changed && mode_value != current_mode)
  {
    current_mode = mode_value;

    // Reset the newly activated scaler so it does not inherit stale stats.
    if(current_mode == Mode::Min_max)
      minmax.reset();
    else
      quantile.reset();
  }

  // --- handle time window changes (shared between both scalers) ---
  if(infinite_changed || time_changed)
  {
    const double tw_seconds = infinite ? 0.0
                                       : static_cast<double>(tw);
    minmax.timeWindow(tw_seconds);
    quantile.timeWindow(tw_seconds);
  }

  // --- handle quantile span change ---
  if(span_changed)
  {
    quantile.span(span);
  }

  // --- compute dt (seconds) from tick ---
  float dt = 0.0f;
  if(setup.rate > 0.0)
  {
    const float maybe_dt =
        static_cast<float>(t.frames) / static_cast<float>(setup.rate);
    if(maybe_dt > 0.f)
      dt = maybe_dt;
  }

  const float x = inputs.scaling_signal;

  // --- process ---
  float scaled01 = 0.0f;

  if(current_mode == Mode::Min_max)
  {
    scaled01 = minmax.put(x, static_cast<double>(dt));      // expected ∈ [0, 1]
  }
  else
  {
    scaled01 = quantile.put(x, static_cast<double>(dt));    // expected ∈ [0, 1]
  }

  // Map scaled value into chosen output range.
  const float y = helpers::map(
      scaled01,
      0.0f, 1.0f,
      inputs.out_low,
      inputs.out_high);

  outputs.out = y;
}

} // namespace puara_gestures::objects