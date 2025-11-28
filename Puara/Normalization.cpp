#include "Normalization.hpp"

#include <cmath>
#include <limits>

namespace puara_gestures::objects
{
// CV: stddev / mean
// NOTE: Only valid for strictly positive signals.
float Normalization::cv(float mu, float sd, float eps)
{
  if(!(sd > 0.0f))
    return 0.0f;
  if(std::fabs(mu) < eps)
    return 0.0f; // avoid division by zero
  return sd / mu;
}

void Normalization::prepare(halp::setup info)
{
  setup = info;

  norm = Normalizer(Normalizer::kDefaultTargetMean,
                    Normalizer::kDefaultTargetStdDev);

  norm.targetMean(inputs.target_mean);
  norm.targetStdDev(inputs.target_std);

  if(inputs.infinite_time_window)
    norm.timeWindow(0.0f); // 0 => infinite window
  else
    norm.timeWindow(inputs.time_window);

  if(inputs.clamp_enable)
    norm.clamp(inputs.clamp_nsig);
  else
    norm.noClamp();

  // Initialize watchers to current UI state
  mean_watch.last          = inputs.target_mean;
  mean_watch.first         = false;
  std_watch.last           = inputs.target_std;
  std_watch.first          = false;
  time_watch.last          = inputs.time_window;
  time_watch.first         = false;
  clamp_watch.last         = inputs.clamp_nsig;
  clamp_watch.first        = false;
  clamp_enable_watch.last  = inputs.clamp_enable;
  clamp_enable_watch.first = false;
  infinite_watch.last      = inputs.infinite_time_window;
  infinite_watch.first     = false;
}

void Normalization::operator()(halp::tick t)
{
  // live param updates (only when changed)
  const bool mean_changed      = mean_watch.changed(inputs.target_mean);
  const bool std_changed       = std_watch.changed(inputs.target_std);
  const bool time_changed      = time_watch.changed(inputs.time_window);
  const bool clamp_changed     = clamp_watch.changed(inputs.clamp_nsig);
  const bool clamp_en_changed  = clamp_enable_watch.changed(inputs.clamp_enable);
  const bool inf_changed       = infinite_watch.changed(inputs.infinite_time_window);

  if(mean_changed)
    norm.targetMean(inputs.target_mean);

  if(std_changed)
    norm.targetStdDev(inputs.target_std);

  if(time_changed || inf_changed)
    norm.timeWindow(inputs.infinite_time_window ? 0.0f : inputs.time_window);

  if(clamp_en_changed || clamp_changed)
  {
    if(inputs.clamp_enable)
      norm.clamp(inputs.clamp_nsig);
    else
      norm.noClamp();
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

  const float x = inputs.normalization_signal;

  // --- process ---
  const float y = norm.put(x, static_cast<double>(dt));

  // outputs
  outputs.out    = y;
  outputs.mean   = norm.mean();
  outputs.stddev = norm.stddev();

  // Outlier: Plaquette-style — raw value is ≥ N stddev away from running mean
  outputs.outlier = norm.isOutlier(x, inputs.out_thresh);

  // Classic coefficient of variation (positive signals only)
  const float mu = outputs.mean;
  const float sd = outputs.stddev;
  outputs.variation = cv(mu, sd);
}
} // namespace puara_gestures::objects
