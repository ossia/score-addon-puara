#pragma once

#include "halp_utils.hpp"

#include <halp/audio.hpp>
#include <halp/controls.hpp>
#include <halp/meta.hpp>

namespace puara_gestures::objects
{
class Smoother
{
public:
  halp_meta(name, "Smoother")
  halp_meta(category, "Analysis/Data")
  halp_meta(c_name, "Smoother")
  halp_meta(
      description,
      "Real-time signal smoothing using an exponential moving average (EMA). "
      "Smoothing can be controlled via alpha, cutoff (Hz), or time window τ (s). "
      "These parameters represent the same filter behaviour and are internally "
      "linked: whichever you move last defines the actual smoothing.")
  halp_meta(author, "Luana Belinsky")
  halp_meta(manual_url, "https://ossia.io/score-docs/")
  halp_meta(uuid, "2a664823-b7d5-41c7-aed2-8afc3d7d0e79")

  struct
  {
    halp::data_port<"Signal", "Input signal to smooth", float> smooth_signal;

    // Direct EMA coefficient
    halp::hslider_f32<"Alpha", halp::range{0.0001, 1.0, 0.5}> alpha;

    // Time constant (seconds)
    halp::spinbox_f32<"Time window τ (s)", halp::range{0.001, 1000.0, 1.0}> tau;

    // 1-pole low-pass cutoff (Hz)
    halp::spinbox_f32<"Cutoff frequency (Hz)", halp::range{0.01, 2000.0, 2.0}> cutoff;
  } inputs;

  struct
  {
    halp::data_port<"Smoothed", "Smoothed output signal.", float> out;
  } outputs;

  halp::setup setup;
  void prepare(halp::setup info);

  using tick = halp::tick;
  void operator()(halp::tick t);

private:
  // --- EMA conversions (with dt in seconds) ---
  static float alpha_from_tau(float tau_s, float dt);
  static float tau_from_alpha(float alpha, float dt);
  static float alpha_from_cutoff(float fc_hz, float dt);
  static float cutoff_from_alpha(float alpha, float dt);

  // --- internal EMA state ---
  float filtered{0.0f};
  bool  has_filtered{false};

  // Effective smoothing coefficient (what the filter actually uses)
  float alpha_eff{0.5f};

  // Parameter watchers (on raw float values)
  halp::ParameterWatcher<float> alpha_watcher;
  halp::ParameterWatcher<float> tau_watcher;
  halp::ParameterWatcher<float> cutoff_watcher;

  bool params_initialized{false};
};

} // namespace puara_gestures::objects
