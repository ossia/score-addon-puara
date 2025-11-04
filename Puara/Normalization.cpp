
#include "Normalization.hpp"
#include <limits>
#include <cmath>

namespace puara_gestures::objects
{
// Adaptive CV: classic stddev/|mean| when |mean| is “large enough”, otherwise stddev/RMS.
// - eps: hard floor to avoid divide-by-zero
// - rel: how “large enough” |mean| must be vs stddev to stay in classic mode
inline float adaptive_cv(float mu, float sd,
                         float eps = 1e-6f,
                         float rel = 5e-2f)  // 0.05 default
{
  if (!(sd > 0.0f)) return 0.0f;

  const float abs_mu = std::fabs(mu);
  // If |mean| ≥ rel*stddev + eps → classic CV
  if (abs_mu >= rel * sd + eps) {
    return sd / abs_mu;
  }

  // Otherwise use RMS CV (bounded, well-defined even when mean≈0)
  const float rms2 = sd * sd + mu * mu;     // E[x^2] = Var + mean^2
  const float rms  = std::sqrt(rms2);
  if (!(rms > 0.0f)) return 0.0f;
  return sd / rms;                           // ∈ [0, 1]
}

void Normalization::prepare(halp::setup info)
{
  setup = info;

  norm = Normalizer(Normalizer::kDefaultTargetMean,
                    Normalizer::kDefaultTargetStdDev);

  norm.targetMean(inputs.target_mean);
  norm.targetStdDev(inputs.target_std);

  if (inputs.infinite_time_window)
    norm.timeWindow(0.0f);          // 0 => infinite window
  else
    norm.timeWindow(inputs.time_window);

  if (inputs.clamp_enable)
    norm.clamp(inputs.clamp_nsig);
  else
    norm.noClamp();
}

void Normalization::operator()(halp::tick t)
{
  // live param updates
  norm.targetMean(inputs.target_mean);
  norm.targetStdDev(inputs.target_std);
  norm.timeWindow(inputs.infinite_time_window ? 0.0f : inputs.time_window);

  if (inputs.clamp_enable) norm.clamp(inputs.clamp_nsig); else norm.noClamp();

  // --- compute dt (seconds) from tick ---
  float dt = 0;
  if (setup.rate > 0.0) {
    const float maybe_dt = static_cast<float>(t.frames) / static_cast<float>(setup.rate);
    if (maybe_dt > 0.f && maybe_dt < 0.1f)  // guard: 0 < dt < 100 ms
      dt = maybe_dt;
  }

  // --- process ---
  const float y = norm.put(inputs.normalization_signal, static_cast<double>(dt));

  // outputs
  outputs.out     = y;
  outputs.mean    = norm.mean();
  outputs.stddev  = norm.stddev();
  outputs.outlier = norm.isOutlier(y, inputs.out_thresh);

  // Adaptive CV: classic when |mean| big enough, RMS when |mean| small
  const float mu  = outputs.mean;
  const float sd  = outputs.stddev;
  outputs.variation = adaptive_cv(mu, sd, 1e-6f, inputs.cv_rel);
}
} // namespace