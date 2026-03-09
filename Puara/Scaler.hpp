#pragma once

#include "3rdparty/extras/MinMaxScaler.h"
#include "3rdparty/extras/QuantileScaler.h"
#include "3rdparty/extras/helpers.h"

#include <limits>
#include <cstdint>

#include <halp/audio.hpp>
#include <halp/controls.hpp>
#include <halp/meta.hpp>
#include <puara/gestures.h>
#include "halp_utils.hpp"
namespace puara_gestures::objects
{

class Scaler
{
public:
  halp_meta(name, "Scaler")
  halp_meta(category, "Analysis/Data")
  halp_meta(c_name, "Scaler")
  halp_meta(author, "Luana Belinsky (adapted from Sofian Audry’s Plaquette)")
  halp_meta(
      description,
      "Adaptive scaler with min-max or quantile-based modes. "
      "Maps input into a chosen output range using a time window.")
  halp_meta(manual_url, "https://plaquette.org/MinMaxScaler.html")
  halp_meta(uuid, "0c1f93f9-de5b-4535-8f70-c646c05dcb08")

  enum class Mode
  {
    Min_max,
    Quantile
  };

  struct
  {
    halp::data_port<
        "Signal",
        "Input signal to scale",
        float>
        scaling_signal;

    halp::enum_t<Mode, "Mode"> mode{Mode::Min_max};

    halp::spinbox_f32<"Time window (s)", halp::range{0.01, 360.0, 1.0}> time_window;

    halp::toggle<
        "Infinite time window">
        infinite_time_window{false};

    halp::spinbox_f32<
        "Output low",
        halp::range{-5.0, 5.0, 0.0}>
        out_low;

    halp::spinbox_f32<
        "Output high",
        halp::range{-5.0, 5.0, 1.0}>
        out_high;

    halp::knob_f32<
        "Span",
        halp::range{0.50, 1.00, 0.99}>
        span;
  } inputs;

  struct
  {
    halp::data_port<
        "Scaled",
        "Signal scaled into the chosen output range.",
        float>
        out;
  } outputs;

  halp::setup setup;

  void prepare(halp::setup info)
  {
    setup = info;

    // Initialize both scalers with default config.
    minmax = MinMaxScaler();     // infinite window by default
    quantile = QuantileScaler(); // infinite window by default

    // Initial time window setup from UI.
    const bool infinite = inputs.infinite_time_window;
    const float tw = inputs.time_window;

    const double tw_seconds = infinite ? 0.0 : static_cast<double>(tw);

    minmax.timeWindow(tw_seconds);
    quantile.timeWindow(tw_seconds);

    // Initial span for quantile mode.
    quantile.span(inputs.span);

    // Initial mode from UI.
    Mode mode_value = inputs.mode.value;
    current_mode = mode_value;

    // Prime watchers with initial values
    mode_watcher.changed(mode_value);
    infinite_watcher.changed(infinite);
    time_window_watcher.changed(tw);
    span_watcher.changed(inputs.span);
  }

  using tick = halp::tick;

  void operator()(halp::tick t)
  {
    // --- live param updates (only when changed) ---
    Mode mode_value = static_cast<Mode>(inputs.mode);
    const bool mode_changed = mode_watcher.changed(mode_value);

    const bool infinite = inputs.infinite_time_window;
    const float tw = inputs.time_window;
    const bool infinite_changed = infinite_watcher.changed(infinite);
    const bool time_changed = time_window_watcher.changed(tw);

    const float span = inputs.span;
    const bool span_changed = span_watcher.changed(span);

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
      const double tw_seconds = infinite ? 0.0 : static_cast<double>(tw);
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
      const float maybe_dt
          = static_cast<float>(t.frames) / static_cast<float>(setup.rate);
      if(maybe_dt > 0.f)
        dt = maybe_dt;
    }

    const float x = inputs.scaling_signal;

    // --- process ---
    float scaled01 = 0.0f;

    if(current_mode == Mode::Min_max)
    {
      scaled01 = minmax.put(x, static_cast<double>(dt)); // expected ∈ [0, 1]
    }
    else
    {
      scaled01 = quantile.put(x, static_cast<double>(dt)); // expected ∈ [0, 1]
    }

    // Map scaled value into chosen output range.
    const float y = helpers::map(scaled01, 0.0f, 1.0f, inputs.out_low, inputs.out_high);

    outputs.out = y;
  }

  // Internal scaling engines.
  MinMaxScaler   minmax;
  QuantileScaler quantile;

  // Currently active mode.
  Mode current_mode{Mode::Min_max};

  // Parameter watchers.
  halp::ParameterWatcher<Mode>  mode_watcher;
  halp::ParameterWatcher<bool>  infinite_watcher;
  halp::ParameterWatcher<float> time_window_watcher;
  halp::ParameterWatcher<float> span_watcher;
};

} // namespace puara_gestures::objects
