#pragma once

#include <halp/controls.hpp>
#include <halp/meta.hpp>
#include <vector>

namespace puara_gestures::objects
{

class PowerBandAvnd
{
public:
  halp_meta(name, "Power Band")
  halp_meta(category, "Analysis/Data processing")
  halp_meta(c_name, "puara_powerband_avnd")
  halp_meta(description, "Computes the power within a specific frequency band from a PSD.")
  halp_meta(manual_url, "https://github.com/dav0dea/goofi-pipe")
  halp_meta(uuid, "defc59ec-4067-4430-aef3-e95cfd2b870f")

  enum class PowerType { Absolute, Relative };

  struct ins
  {
    // The Power Spectral Density values
    halp::val_port<"PSD", std::vector<double>> psd;
    // The frequency value corresponding to each PSD point
    halp::val_port<"Frequencies", std::vector<double>> frequencies;

    // --- Parameters ---
    halp::knob_f32<"Min Frequency (Hz)", halp::range{0.01, 9999.0, 1.0}> f_min;
    halp::knob_f32<"Max Frequency (Hz)", halp::range{1.0, 10000.0, 60.0}> f_max;
    halp::enum_t<PowerType, "Power Type"> power_type{PowerType::Absolute};
  } inputs;

  struct outs
  {
    // The final computed power is a single value
    halp::val_port<"Power", double> power;
  } outputs;

  void operator()();
};

}
