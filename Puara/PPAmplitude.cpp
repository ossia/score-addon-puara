#include "PPAmplitude.hpp"

namespace puara_gestures::objects
{

void PeakAmplitude::prepare(halp::setup info)
{
  setup = info;

  last_min_       = 0.0f;
  last_max_       = 0.0f;
  last_amplitude_ = 0.0f;

  have_min_       = false;
  have_max_       = false;

  prev_min_gate_  = false;
  prev_max_gate_  = false;

  // Prime mode watcher so the first tick doesn't treat it as a spurious change.
  mode_watcher.changed(inputs.mode.value);
}

void PeakAmplitude::operator()(halp::tick)
{
  const float raw      = inputs.raw_signal;
  const bool  min_gate = inputs.peak_min_gate;
  const bool  max_gate = inputs.peak_max_gate;

  Mode mode_value = inputs.mode.value;

  // Handle mode change: if mode flips (OnMax <-> OnMin), reset the cycle state
  // so we don't mix a "last min" from the previous mode with a "new max", etc.
  if (mode_watcher.changed(mode_value))
  {
    have_min_       = false;
    have_max_       = false;
    last_min_       = raw;
    last_max_       = raw;
    last_amplitude_ = 0.0f;
    prev_min_gate_  = min_gate;
    prev_max_gate_  = max_gate;
  }

  // Rising-edge detection on the gates.
  const bool min_rise = (min_gate && !prev_min_gate_);
  const bool max_rise = (max_gate && !prev_max_gate_);

  prev_min_gate_ = min_gate;
  prev_max_gate_ = max_gate;

  // Latch raw values at the exact peak events.
  if (min_rise)
  {
    last_min_ = raw;
    have_min_ = true;
  }

  if (max_rise)
  {
    last_max_ = raw;
    have_max_ = true;
  }

  float amp = last_amplitude_; // hold previous amplitude by default

  const bool strict = inputs.strict_cycles;

  // Emit amplitude depending on the selected mode and available extrema.
  switch (mode_value)
  {
  case Mode::OnMax:
    // When a max event occurs and we have already seen a min for this cycle:
    // amplitude = max_raw - last_min_raw
    if (max_rise && have_min_)
    {
      amp = last_max_ - last_min_;
      last_amplitude_ = amp;

      // Strict min→max→min cycles: force a new min before next amplitude.
      if (strict)
        have_min_ = false;
    }
    break;

  case Mode::OnMin:
    // When a min event occurs and we have already seen a max for this cycle:
    // amplitude = last_max_raw - min_raw
    if (min_rise && have_max_)
    {
      amp = last_max_ - last_min_;
      last_amplitude_ = amp;

      // Strict max→min→max cycles: force a new max before next amplitude.
      if (strict)
        have_max_ = false;
    }
    break;
  }

  outputs.amplitude = amp;
}

} // namespace puara_gestures::objects
