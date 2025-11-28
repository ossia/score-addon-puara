#pragma once
#include "3rdparty/extras/Normalizer.h"
#include "halp_utils.hpp"

#include <halp/audio.hpp>
#include <halp/controls.hpp>
#include <halp/meta.hpp>
#include <puara/gestures.h>

#include <limits>

namespace puara_gestures::objects
{
// normalizer
class Normalization
{
public:
  halp_meta(name, "Normalizer")
  halp_meta(category, "Analysis/Data")
  halp_meta(c_name, "Normalizer")
  halp_meta(author, "Luana Belinsky (adapted from Sofian Audry’s Plaquette)")
  halp_meta(
      description,
      "Adaptive normalizer with a time window (seconds). "
      "Tracks running mean and standard deviation of the input and "
      "maps it to a target mean & standard deviation.\n"
      "Outliers are detected on the raw signal as N standard deviations away "
      "from its running mean (Plaquette-style).\n"
      "Note: Coefficient of variation (CV) is computed as stddev / mean and "
      "assumes a strictly positive signal.")
  halp_meta(manual_url, "https://plaquette.org/Normalizer.html")
  halp_meta(uuid, "c6f3f2d3-6f37-4e3f-8db6-0a5a4d9f7a01")

  struct
  {
    halp::data_port<
        "Signal",
        "Input signal to normalize",
        float>
        normalization_signal;

    halp::knob_f32<
        "Target mean",
        halp::range{0.0, 1.0, 0.5}>
        target_mean; // Desired output mean after normalization

    halp::knob_f32<
        "Target std dev",
        halp::range{0.0, 10.0, 0.15}>
        target_std; // Desired output standard deviation (range expanded for flexibility)

    halp::knob_f32<
        "Time window (s)",
        halp::range{0.01, 360.0, 1.0}>
        time_window; // EMA window for stats (seconds)

    halp::toggle<
        "Infinite time window">
        infinite_time_window{false}; // Track stats over all time (no decay)

    halp::knob_f32<
        "Outlier threshold",
        halp::range{0.0, 10.0, 1.5}>
        out_thresh; // N·stddev used to flag outliers (in raw-signal stats space)

    halp::toggle<
        "Clamp output">
        clamp_enable{true}; // Limit output to target mean ± N·target stddev

    halp::knob_f32<
        "Clamp max",
        halp::range{0.10, 5.00, 3.33}>
        clamp_nsig; // N·target stddev used for clamp range

  } inputs;

  struct
  {
    halp::data_port<
        "Normalized signal",
        "Float. Input remapped to the target mean and standard deviation.",
        float>
        out;

    halp::data_port<
        "Mean",
        "Float. Current running mean of the input signal.",
        float>
        mean;

    halp::data_port<
        "Standard deviation",
        "Float. Current standard deviation of the input signal.",
        float>
        stddev;

    halp::data_port<
        "Outlier",
        "Boolean. True when the raw signal is at least N standard deviations "
        "away from its current running mean (based on the time window).",
        bool>
        outlier;

    halp::data_port<
        "Coefficient of variation",
        "Float. Ratio of standard deviation to mean.\n"
        "Only valid for strictly positive signals.",
        float>
        variation;
  } outputs;

  halp::setup setup;
  void prepare(halp::setup info);

  using tick = halp::tick;
  void operator()(halp::tick t);

private:
  Normalizer norm{Normalizer::kDefaultTargetMean,
                  Normalizer::kDefaultTargetStdDev};

  // Classic CV: stddev / mean (only valid for positive signals)
  float cv(float mu, float sd, float eps = 1e-6f);

  // Parameter watchers
  halp::ParameterWatcher<float> mean_watch;
  halp::ParameterWatcher<float> std_watch;
  halp::ParameterWatcher<float> time_watch;
  halp::ParameterWatcher<float> clamp_watch;
  halp::ParameterWatcher<bool>  clamp_enable_watch;
  halp::ParameterWatcher<bool>  infinite_watch;
};
} // namespace puara_gestures::objects
