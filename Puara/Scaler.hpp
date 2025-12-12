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

    halp::spinbox_f32<
        "Time window (s)",
        halp::range{0.01, 360.0, 1.0}>
        time_window;

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
  void prepare(halp::setup info);

  using tick = halp::tick;
  void operator()(halp::tick t);

private:
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