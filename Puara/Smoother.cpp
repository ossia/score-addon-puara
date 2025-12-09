#include "Smoother.hpp"

#include "3rdparty/extras/EMA.h"

#include <cmath>

#include <algorithm>
#include <limits>

namespace puara_gestures::objects
{

// Convert alpha to time window: τ = -dt / ln(1-α)
float Smoother::alpha_to_time_window(float alpha, float dt)
{
  if(dt <= 0.0f)
    return std::numeric_limits<float>::infinity();
  if(alpha >= 1.0f)
    return 0.0f;
  if(alpha <= 0.0f)
    return std::numeric_limits<float>::infinity();

  float arg = 1.0f - alpha;
  float denom = std::log(arg);
  return -dt / denom;
}

// Convert time window to alpha: α = 1 - exp(-dt/τ)
float Smoother::time_window_to_alpha(float tau_s, float dt)
{
  if(dt <= 0.0f)
    return 0.5f;
  if(tau_s <= 0.0f)
    return 1.0f;
  if(tau_s == std::numeric_limits<float>::infinity())
    return 0.0f;

  float exponent = -dt / tau_s;
  return 1.0f - std::exp(exponent);
}

// Convert alpha to cutoff: fc = -ln(1-α) / (2π·dt)
float Smoother::alpha_to_cutoff(float alpha, float dt)
{
  if(dt <= 0.0f)
    return 0.0f;
  if(alpha >= 1.0f)
    return std::numeric_limits<float>::infinity();
  if(alpha <= 0.0f)
    return 0.0f;

  float arg = 1.0f - alpha;
  float denom = std::log(arg);
  return -denom / (2.0f * float(M_PI) * dt);
}

// Convert cutoff to alpha: α = 1 - exp(-2π·fc·dt)
float Smoother::cutoff_to_alpha(float fc_hz, float dt)
{
  if(dt <= 0.0f)
    return 0.0f;
  if(fc_hz <= 0.0f)
    return 0.0f;

  float exponent = -2.0f * float(M_PI) * fc_hz * dt;
  return 1.0f - std::exp(exponent);
}

// Convert time window to cutoff (continuous-time): fc = 1/(2π·τ)
float Smoother::time_window_to_cutoff(float tau_s)
{
  if(tau_s <= 0.0f)
    return std::numeric_limits<float>::infinity();
  if(tau_s == std::numeric_limits<float>::infinity())
    return 0.0f;

  return 1.0f / (2.0f * float(M_PI) * tau_s);
}

// Convert cutoff to time window (continuous-time): τ = 1/(2π·fc)
float Smoother::cutoff_to_time_window(float fc_hz)
{
  if(fc_hz <= 0.0f)
    return std::numeric_limits<float>::infinity();
  if(fc_hz == std::numeric_limits<float>::infinity())
    return 0.0f;

  return 1.0f / (2.0f * float(M_PI) * fc_hz);
}
void Smoother::prepare(halp::setup info)
{
  setup = info;

  // Reset filter state
  filtered = 0.0f;
  has_filtered = false;
  sample_count = 0;
  params_initialized = false;
  current_tau = 1.0f;

  // Default to alpha control
  last_modified = Parameter::Alpha;
  last_dt = DEFAULT_DT;

  // Initialize parameter watchers
  cumulative_watcher.last = inputs.cumulative;
  cumulative_watcher.first = false;
  alpha_watcher.last = inputs.alpha;
  alpha_watcher.first = false;
  tau_watcher.last = inputs.tau;
  tau_watcher.first = false;
  cutoff_watcher.last = inputs.cutoff;
  cutoff_watcher.first = false;

  // Initial alpha value
  alpha_eff = std::clamp(static_cast<float>(inputs.alpha), 0.0f, 1.0f);
}

void Smoother::operator()(halp::tick t)
{
  // Calculate time step
  float dt = DEFAULT_DT;
  if(setup.rate > 0.0)
  {
    const float maybe_dt = static_cast<float>(t.frames) / static_cast<float>(setup.rate);
    if(maybe_dt > 0.0f && maybe_dt < 0.1f)
      dt = maybe_dt;
  }

  // Read current UI values
  bool cumulative_ui = static_cast<bool>(inputs.cumulative);
  float alpha_ui = static_cast<float>(inputs.alpha);
  float tau_ui = static_cast<float>(inputs.tau);
  float cutoff_ui = static_cast<float>(inputs.cutoff);

  // Detect parameter changes
  const bool cumulative_changed = cumulative_watcher.changed(cumulative_ui);
  const bool alpha_changed = alpha_watcher.changed(alpha_ui);
  const bool tau_changed = tau_watcher.changed(tau_ui);
  const bool cutoff_changed = cutoff_watcher.changed(cutoff_ui);

  // Update last_modified based on changes
  if(cumulative_changed)
  {
    last_modified = Parameter::Tau;

    // When turning cumulative OFF, restore last non-cumulative parameter
    if(!cumulative_ui && last_non_cumulative_param != Parameter::None)
    {
      last_modified = last_non_cumulative_param;
    }
  }

  if(alpha_changed)
  {
    last_modified = Parameter::Alpha;
    last_non_cumulative_param = Parameter::Alpha;
  }

  if(tau_changed)
  {
    last_modified = Parameter::Tau;
    last_non_cumulative_param = Parameter::Tau;
  }

  if(cutoff_changed)
  {
    last_modified = Parameter::Cutoff;
    last_non_cumulative_param = Parameter::Cutoff;
  }

  // Calculate exact values based on last modified parameter
  float exact_alpha = 0.5f;
  float exact_tau = 1.0f;

  if(!cumulative_ui)
  {
    // Use last-modified parameter
    switch(last_modified)
    {
      case Parameter::Alpha:
        // User modified alpha - use it directly
        exact_alpha = std::clamp(alpha_ui, 0.0f, 1.0f);
        exact_tau = alpha_to_time_window(exact_alpha, dt);
        break;
      case Parameter::Tau:
        // User modified tau - calculate alpha from tau
        exact_tau = tau_ui;
        exact_alpha = time_window_to_alpha(exact_tau, dt);
        break;
      case Parameter::Cutoff:
        // User modified cutoff - calculate alpha from cutoff
        exact_tau = cutoff_to_time_window(cutoff_ui);
        exact_alpha = cutoff_to_alpha(cutoff_ui, dt);
        break;
      default:
        // Default to alpha
        exact_alpha = std::clamp(alpha_ui, 0.0f, 1.0f);
        exact_tau = alpha_to_time_window(exact_alpha, dt);
        break;
    }

    alpha_eff = exact_alpha;
    current_tau = exact_tau;
  }

  // Apply filtering
  const float input_signal = inputs.smooth_signal;

  if(cumulative_ui)
  {
    // Cumulative mode: α = 1/(n+1)
    float alpha = ema_alpha(true, -1.0, sample_count, dt);

    if(!has_filtered)
    {
      filtered = input_signal;
      has_filtered = true;
    }
    else
    {
      ema_apply_update(filtered, input_signal, alpha);
    }

    sample_count++;

#if ENABLE_PARAMETER_LINKING
    // Show actual alpha in UI
    inputs.alpha.value = alpha;
#endif
  }
  else
  {
    // Finite mode: exact EMA
    if(!has_filtered)
    {
      filtered = input_signal;
      has_filtered = true;
    }
    else
    {
      filtered = alpha_eff * input_signal + (1.0f - alpha_eff) * filtered;
    }

    sample_count = 0;
  }

// Update parameter displays when needed
#if ENABLE_PARAMETER_LINKING
  const float dt_change_threshold = 0.000001f;
  const bool dt_changed = (std::abs(dt - last_dt) > dt_change_threshold);

  // Update displays when dt changes or parameters change
  if(dt_changed || alpha_changed || tau_changed || cutoff_changed || cumulative_changed)
  {
    if(cumulative_ui)
    {
      // Cumulative mode display
      inputs.tau.value = MAX_TAU;
      inputs.cutoff.value = MIN_CUTOFF;
    }
    else
    {
      // Update displays
      inputs.alpha.value = exact_alpha;

      // Handle tau display
      if(!std::isfinite(exact_tau) || exact_tau < 0.0f)
      {
        inputs.tau.value = MAX_TAU;
        // Auto-enable cumulative for very small alpha
        if(exact_alpha <= 0.0001f)
        {
          inputs.cumulative.value = true;
          sample_count = 0;
        }
      }
      else
      {
        inputs.tau.value = exact_tau;
      }

      // Handle cutoff display
      float cutoff_val = time_window_to_cutoff(exact_tau);
      if(!std::isfinite(cutoff_val) || cutoff_val < 0.0f)
      {
        inputs.cutoff.value = MIN_CUTOFF;
      }
      else
      {
        inputs.cutoff.value = std::clamp(cutoff_val, MIN_CUTOFF, MAX_CUTOFF);
      }
    }

    last_dt = dt;
  }
#endif

  outputs.out = filtered;
}
} // namespace puara_gestures::objects