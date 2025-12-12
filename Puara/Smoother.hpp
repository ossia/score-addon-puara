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
  halp_meta(category, "Filters/Effects")
  halp_meta(c_name, "Smoother")
  halp_meta(
      description,
      "Smooth signals with an exponential moving average filter. "
      "Alpha: direct smoothing coefficient (0=frozen, 1=no smoothing). "
      "Tau: time constant in seconds. "
      "Cutoff: equivalent low-pass frequency in Hz. "
      "Cumulative: infinite-window averaging (α = 1/(n+1)). "
      "Parameters are linked - changing one updates the others.")
  halp_meta(author, "Luana Belinsky")
  halp_meta(manual_url, "https://ossia.io/score-docs/")
  halp_meta(uuid, "2a664823-b7d5-41c7-aed2-8afc3d7d0e79")

  // Enable/disable parameter linking while waiting for halp implementation
  static constexpr bool ENABLE_PARAMETER_LINKING = false;

  // Display limits
  static constexpr float MAX_TAU = 1000.0f;
  static constexpr float MAX_CUTOFF = 2000.0f;
  static constexpr float MIN_CUTOFF = 0.01f;

  struct
  {
    halp::data_port<"Signal", "Input signal to smooth (int or float).", float>
        smooth_signal;

    // Cumulative averaging toggle
    struct : halp::toggle<"Cumulative average", halp::toggle_setup{false}>
    {
#if ENABLE_PARAMETER_LINKING
      static void on_controller_interaction(Smoother& object, bool value)
      {
        object.inputs.cumulative.value = value;
        if(value)
          object.sample_count = 0;
      }
#endif
    } cumulative;

    // Alpha control - direct smoothing coefficient
    struct : halp::hslider_f32<"Alpha coefficient", halp::range{0.0, 1.0, 0.5}>
    {
#if ENABLE_PARAMETER_LINKING
      static void on_controller_interaction(Smoother& object, float value)
      {
        object.inputs.alpha.value = value;
        object.last_modified = Parameter::Alpha;

        if(value <= 0.0f)
        {
          object.inputs.cumulative.value = true;
          object.sample_count = 0;
        }
        else
        {
          object.inputs.cumulative.value = false;
        }
      }
#endif
    } alpha;

    // Time constant in seconds
    struct : halp::spinbox_f32<"Time window tau (s)", halp::range{0.0, MAX_TAU, 1.0}>
    {
#if ENABLE_PARAMETER_LINKING
      static void on_controller_interaction(Smoother& object, float value)
      {
        object.inputs.tau.value = value;
        object.last_modified = Parameter::Tau;

        if(value >= MAX_TAU)
        {
          object.inputs.cumulative.value = true;
          object.sample_count = 0;
        }
        else
        {
          object.inputs.cumulative.value = false;
        }
      }
#endif
    } tau;

    // Cutoff frequency in Hz
    struct
        : halp::spinbox_f32<"Cutoff frequency (Hz)", halp::range{0.0, MAX_CUTOFF, 2.0}>
    {
#if ENABLE_PARAMETER_LINKING
      static void on_controller_interaction(Smoother& object, float value)
      {
        object.inputs.cutoff.value = value;
        object.last_modified = Parameter::Cutoff;

        if(value <= 0.0f)
        {
          object.inputs.cumulative.value = true;
          object.sample_count = 0;
        }
        else
        {
          object.inputs.cumulative.value = false;
        }
      }
#endif
    } cutoff;
  } inputs;

  struct
  {
    halp::data_port<"Smoothed", "Smoothed output signal (float).", float> out;
  } outputs;

  halp::setup setup;
  void prepare(halp::setup info);

  using tick = halp::tick;
  void operator()(halp::tick t);

private:
  // Parameter conversion functions
  static float alpha_to_time_window(float alpha, float dt);
  static float time_window_to_alpha(float tau_s, float dt);
  static float alpha_to_cutoff(float alpha, float dt);
  static float cutoff_to_alpha(float fc_hz, float dt);
  static float time_window_to_cutoff(float tau_s);
  static float cutoff_to_time_window(float fc_hz);

  // Internal filter state
  float filtered{0.0f};
  bool has_filtered{false};

  // Sample counter for cumulative mode
  uint32_t sample_count{0};

  // Current effective alpha for filtering
  float alpha_eff{0.5f};

  // Current time window
  float current_tau{1.0f};

  // Parameter watchers for manual change detection
  halp::ParameterWatcher<bool> cumulative_watcher;
  halp::ParameterWatcher<float> alpha_watcher;
  halp::ParameterWatcher<float> tau_watcher;
  halp::ParameterWatcher<float> cutoff_watcher;

  // Track which parameter was last modified
  enum class Parameter
  {
    Alpha,
    Tau,
    Cutoff,
    None
  };
  Parameter last_modified{Parameter::Alpha};

  // Track last non-cumulative parameter (for when cumulative is unchecked)
  Parameter last_non_cumulative_param{Parameter::Alpha};

  // First-time setup flag
  bool params_initialized{false};

  // Default time step (0.01s = 100Hz)
  static constexpr float DEFAULT_DT = 0.01f;

  // Last calculated time step
  float last_dt{DEFAULT_DT};
};
}