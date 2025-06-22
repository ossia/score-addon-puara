#pragma once

#include "statistics_algorithms.hpp"

#include <halp/controls.hpp>
#include <halp/meta.hpp>

#include <vector>

namespace puara_gestures::objects
{

class PowerBandEEGAvnd
{
public:
  halp_meta(name, "Power Band EEG")
  halp_meta(category, "Analysis/Puara")
  halp_meta(c_name, "puara_powerbandeeg_avnd")
  halp_meta(
      description,
      "Computes power for standard EEG frequency bands (Delta, Theta, Alpha, etc.).")
  halp_meta(manual_url, "https://github.com/dav0dea/goofi-pipe")
  halp_meta(uuid, "d9a95cf8-b952-412e-882c-f5ebd96ace36")

  struct ins
  {
    halp::val_port<"PSD", std::vector<double>> psd;
    halp::val_port<"Frequencies", std::vector<double>> frequencies;
    halp::enum_t<algorithms::PowerBandType, "Power Type"> power_type{
        algorithms::PowerBandType::Absolute};
  } inputs;

  struct outs
  {
    halp::val_port<"Delta (1-3 Hz)", double> delta;
    halp::val_port<"Theta (3-7 Hz)", double> theta;
    halp::val_port<"Alpha (7-12 Hz)", double> alpha;
    halp::val_port<"Low Beta (12-20 Hz)", double> low_beta;
    halp::val_port<"High Beta (20-30 Hz)", double> high_beta;
    halp::val_port<"Gamma (30-50 Hz)", double> gamma;
  } outputs;

  void operator()();
};

}
