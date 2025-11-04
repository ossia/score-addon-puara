
#pragma once
#include "3rdparty/extras/Normalizer.h"

#include <limits>
#include <halp/audio.hpp>
#include <halp/controls.hpp>
#include <halp/meta.hpp>
#include <puara/gestures.h>
#include "halp_utils.hpp"

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
  halp_meta(description,
    "Adaptive normalizer with a time window (seconds). "
    "Maps input to a target mean & standard deviation.")
  halp_meta(manual_url, "https://plaquette.org/Normalizer.html")
  halp_meta(uuid, "c6f3f2d3-6f37-4e3f-8db6-0a5a4d9f7a01")

struct
{
  halp::data_port<
      "Signal",
      "Input signal to normalize",
      float>
      normalization_signal;

  halp::knob_f32<"Target mean",
                  halp::range{0.0, 1.0, 0.5}>
      target_mean; //Desired output mean after normalization

  halp::knob_f32<"Target std dev",
                  halp::range{0.0, 5.0, 0.15}>
      target_std; //Desired output standard deviation

  halp::knob_f32<"Time window (s)",
                  halp::range{0.01, 360.0, 1.0}>
      time_window; //EMA window for stats (seconds)

  halp::toggle<"Infinite time window">
      infinite_time_window{false}; // Track stats over all time (no decay)

  halp::knob_f32<"Outlier threshold",
                  halp::range{0.0, 10.0, 1.5}>
      out_thresh; //n·stddev used to flag outliers

  halp::toggle<"Clamp output">
      clamp_enable{true}; //Limit output to mean ± n·stddev

  halp::knob_f32<"Clamp max",
                  halp::range{0.10, 5.00, 3.33}>
      clamp_nsig; //n·stddev used for clamp range

  halp::knob_f32<"CV switch sensitivity",
                  halp::range{0.0, 0.5, 0.05}>
      cv_rel; //Min relative change to toggle CV
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
        "Boolean. True when the normalized value exceeds the outlier limits set as ±N standard deviations from the target mean.",
        bool>
        outlier;

    halp::data_port<
        "Coefficient of variation",
        "Float. Ratio of standard deviation to mean.\n"
        "Useful to track relative variability independent of scale.",
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

};
} // namespace
