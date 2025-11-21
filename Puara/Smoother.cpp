#include "Smoother.hpp"

#include <cmath>
#include <algorithm>
#include <limits>

namespace puara_gestures::objects
{

// tau (seconds) -> alpha
float Smoother::alpha_from_tau(float tau_s, float dt)
{
  if (tau_s <= 0.0f || dt <= 0.0f)
    return 1.0f; // immediate response

  // EMA equivalent of continuous-time 1st-order low-pass:
  // alpha = 1 - exp(-dt / tau)
  return 1.0f - std::exp(-dt / tau_s);
}

// alpha -> tau (seconds)
float Smoother::tau_from_alpha(float alpha, float dt)
{
  if (dt <= 0.0f)
    return std::numeric_limits<float>::infinity();

  if (alpha <= 0.0f)
    return std::numeric_limits<float>::infinity(); // extremely slow
  if (alpha >= 1.0f)
    return 0.0f; // immediate

  const float denom = std::log(1.0f - alpha);
  // tau = -dt / ln(1 - alpha)
  return -dt / denom;
}

// cutoff (Hz) -> alpha
float Smoother::alpha_from_cutoff(float fc_hz, float dt)
{
  if (fc_hz <= 0.0f || dt <= 0.0f)
    return 0.0f;

  // 1-pole analog LP: alpha = 1 - exp(-2π fc * dt)
  return 1.0f - std::exp(-2.0f * float(M_PI) * fc_hz * dt);
}

// alpha -> cutoff (Hz)
float Smoother::cutoff_from_alpha(float alpha, float dt)
{
  if (dt <= 0.0f)
    return 0.0f;

  if (alpha <= 0.0f)
    return 0.0f;
  if (alpha >= 1.0f)
    return std::numeric_limits<float>::infinity();

  const float denom = std::log(1.0f - alpha);
  // fc = -ln(1 - alpha) / (2π * dt)
  return -denom / (2.0f * float(M_PI) * dt);
}

void Smoother::prepare(halp::setup info)
{
  setup = info;

  filtered         = 0.0f;
  has_filtered     = false;
  params_initialized = false;

  // Prime watchers with initial parameter *values* (as floats)
  alpha_watcher.changed(static_cast<float>(inputs.alpha));
  tau_watcher.changed(static_cast<float>(inputs.tau));
  cutoff_watcher.changed(static_cast<float>(inputs.cutoff));

  // Initial alpha_eff from alpha knob
  alpha_eff = std::clamp(static_cast<float>(inputs.alpha), 0.0001f, 1.0f);
}

void Smoother::operator()(halp::tick t)
{
  // --- compute dt (seconds) from tick ---
  float dt = 0.0f;
  if (setup.rate > 0.0)
  {
    const float maybe_dt
        = static_cast<float>(t.frames) / static_cast<float>(setup.rate);
    // Guard against absurd values: 0 < dt < 100 ms
    if (maybe_dt > 0.0f && maybe_dt < 0.1f)
      dt = maybe_dt;
  }

  // --- read current UI values as floats ---
  float alpha_ui  = static_cast<float>(inputs.alpha);
  float tau_ui    = static_cast<float>(inputs.tau);     // seconds
  float cutoff_ui = static_cast<float>(inputs.cutoff);  // Hz

  // If dt is unusable, we can only trust alpha directly.
  if (dt <= 0.0f)
  {
    alpha_eff = std::clamp(alpha_ui, 0.0001f, 1.0f);

    const float x = inputs.smooth_signal;
    const float a = alpha_eff;

    if (!has_filtered)
    {
      filtered     = x;
      has_filtered = true;
    }
    else
    {
      filtered = a * x + (1.0f - a) * filtered;
    }

    outputs.out = filtered;
    return;
  }

  // --- detect changes using watchers (on float values) ---
  const bool alpha_changed   = alpha_watcher.changed(alpha_ui);
  const bool tau_changed     = tau_watcher.changed(tau_ui);
  const bool cutoff_changed  = cutoff_watcher.changed(cutoff_ui);

  // --- initialize effective alpha/tau/cutoff on first valid dt ---
  if (!params_initialized)
  {
    // Start from alpha knob as the main control
    alpha_eff = std::clamp(alpha_ui, 0.0001f, 1.0f);
    params_initialized = true;
  }

  // --- keep parameters synchronized internally ---
  // We only need alpha_eff to actually run the filter, but we derive it from
  // whichever parameter the user changed last (priority: alpha > cutoff > tau).

  if (alpha_changed && !cutoff_changed && !tau_changed)
  {
    alpha_eff = std::clamp(alpha_ui, 0.0001f, 1.0f);
  }
  else if (cutoff_changed && !alpha_changed && !tau_changed)
  {
    float a = alpha_from_cutoff(cutoff_ui, dt);
    if (!std::isfinite(a))
      a = 0.0001f;
    alpha_eff = std::clamp(a, 0.0001f, 1.0f);
  }
  else if (tau_changed && !alpha_changed && !cutoff_changed)
  {
    float a = alpha_from_tau(tau_ui, dt);
    if (!std::isfinite(a))
      a = 0.0001f;
    alpha_eff = std::clamp(a, 0.0001f, 1.0f);
  }
  else if (alpha_changed || cutoff_changed || tau_changed)
  {
    // Multiple changed -> priority alpha > cutoff > tau
    if (alpha_changed)
    {
      alpha_eff = std::clamp(alpha_ui, 0.0001f, 1.0f);
    }
    else if (cutoff_changed)
    {
      float a = alpha_from_cutoff(cutoff_ui, dt);
      if (!std::isfinite(a))
        a = 0.0001f;
      alpha_eff = std::clamp(a, 0.0001f, 1.0f);
    }
    else if (tau_changed)
    {
      float a = alpha_from_tau(tau_ui, dt);
      if (!std::isfinite(a))
        a = 0.0001f;
      alpha_eff = std::clamp(a, 0.0001f, 1.0f);
    }
  }
  // else: none changed -> keep previous alpha_eff

  // --- EMA step with effective alpha ---
  const float x = inputs.smooth_signal;
  const float a = alpha_eff;

  if (!has_filtered)
  {
    filtered     = x;
    has_filtered = true;
  }
  else
  {
    filtered = a * x + (1.0f - a) * filtered;
  }

  outputs.out = filtered;
}

} // namespace puara_gestures::objects
