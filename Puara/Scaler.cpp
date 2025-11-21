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
  // --- compute dt (seconds) from tick ---
  float dt = 0.0f;
  if (setup.rate > 0.0f) {
    const float maybe_dt =
        static_cast<float>(t.frames) / static_cast<float>(setup.rate);
    if (maybe_dt > 0.0f && maybe_dt < 0.1f)  // guard: 0 < dt < 100 ms
      dt = maybe_dt;
  }

  // --- handle live parameter changes with watchers ---

  // Mode change (enum_t -> Mode).
  Mode mode_value = static_cast<Mode>(inputs.mode);
  if (mode_watcher.changed(mode_value)) {
    if (mode_value != current_mode) {
      current_mode = mode_value;

      // Reset the newly activated scaler so it does not inherit stale stats.
      if (current_mode == Mode::Min_max)
        minmax.reset();
      else
        quantile.reset();
    }
  }

  // Time window & infinite flag: shared between both scalers.
  const bool  infinite = inputs.infinite_time_window;
  const float tw       = inputs.time_window;

  if (infinite_watcher.changed(infinite) || time_window_watcher.changed(tw)) {
    const double tw_seconds = infinite ? 0.0
                                       : static_cast<double>(tw);
    minmax.timeWindow(tw_seconds);
    quantile.timeWindow(tw_seconds);
  }

  // Quantile span: only affects quantile scaler, but we can watch it here.
  const float span = inputs.span;
  if (span_watcher.changed(span)) {
    quantile.span(span);
  }

  // --- process current sample ---
  const float x = inputs.scaling_signal;

  float scaled01 = 0.0f;

  if (current_mode == Mode::Min_max) {
    scaled01 = minmax.put(x, static_cast<double>(dt));      // expected ∈ [0, 1]
  } else {
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
