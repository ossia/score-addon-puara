#pragma once

#include "halp_utils.hpp"

#include <halp/audio.hpp>
#include <halp/controls.hpp>
#include <halp/meta.hpp>

#include <cstdint>

namespace puara_gestures::objects
{
// amplitude
class PeakAmplitude
{
public:
  halp_meta(name, "Peak-to-peak amplitude")
  halp_meta(category, "Analysis/Data")
  halp_meta(c_name, "peak_amplitude")
  halp_meta(author, "Luana Belinsky")
  halp_meta(
      description,
      "Computes peak-to-peak amplitude in raw units using external peak events.\n"
      "Takes a raw signal and two gates: peak-min and peak-max (from a PeakDetector).\n"
      "You can choose whether to emit an amplitude at each detected peak-max or "
      "peak-min.\n"
      "Amplitude is computed as (raw_peak_max - raw_peak_min) and held until the next "
      "cycle.\n"
      "An optional 'Strict cycles' mode enforces a full min→max→min (or max→min→max) "
      "sequence before emitting the next amplitude.")
  halp_meta(manual_url, "https://ossia.io/score-docs/")
  //   Documentation TODO
  halp_meta(uuid, "f2ffb195-2e4e-4a34-b044-7662107ea0a7")

  /// When to emit the peak-to-peak amplitude.
  enum class Mode
  {
    OnMax, ///< Output amplitude when a peak max event occurs (using last min).
    OnMin  ///< Output amplitude when a peak min event occurs (using last max).
  };

  struct
  {
    halp::data_port<
        "Raw signal",
        "Original, non-normalized signal whose peak-to-peak amplitude will be measured.",
        float>
        raw_signal;

    halp::data_port<
        "Peak min gate",
        "Boolean gate that goes high when a minimum is detected (e.g. from PeakDetector MIN mode).",
        bool>
        peak_min_gate;

    halp::data_port<
        "Peak max gate",
        "Boolean gate that goes high when a maximum is detected (e.g. from PeakDetector MAX mode).",
        bool>
        peak_max_gate;

    // How to define the cycle:
    //  - OnMax:  emit amplitude when a max occurs (cycle min→max→min).
    //  - OnMin:  emit amplitude when a min occurs (cycle max→min→max).
    halp::enum_t<Mode, "Mode"> mode{Mode::OnMax};

    // If enabled, require a full min→max→min or max→min→max sequence
    // before emitting the next amplitude (no reusing old extrema).
    halp::toggle<"Strict cycles"> strict_cycles{true};

  } inputs;

  struct
  {
    halp::data_port<
        "Peak-to-peak amplitude",
        "Peak-to--peak amplitude in raw units, computed as (peak_max_raw - peak_min_raw).\n"
        "Value is updated when a peak event occurs according to the selected mode and held otherwise.",
        float>
        amplitude;
  } outputs;

  halp::setup setup;
  void prepare(halp::setup info);

  using tick = halp::tick;
  void operator()(halp::tick t);

private:
  // Internal state
  float last_min_        = 0.0f;
  float last_max_        = 0.0f;
  float last_amplitude_  = 0.0f;

  bool  have_min_        = false;
  bool  have_max_        = false;

  bool  prev_min_gate_   = false;
  bool  prev_max_gate_   = false;

  // Parameter watcher
  halp::ParameterWatcher<Mode> mode_watcher;
};

} // namespace puara_gestures::objects
