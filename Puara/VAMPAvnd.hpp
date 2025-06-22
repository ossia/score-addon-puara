#pragma once
#include "vamp_algorithms.hpp"
#include <halp/controls.hpp>
#include <halp/meta.hpp>
#include <vector>

namespace puara_gestures::objects
{

class VAMPAvnd
{
public:
  halp_meta(name, "VAMP")
  halp_meta(category, "Analysis/Puara")
  halp_meta(c_name, "puara_vamp_avnd")
  halp_meta(description, "Finds slowest components in a time-series using VAMP.")
  halp_meta(uuid, "3ebf910a-330f-43d8-8958-287b6f191c22")

  struct ins
  {
    halp::val_port<"Data", std::vector<double>> data;
  } inputs;

  struct outs
  {
    halp::val_port<"Components", std::vector<double>> comps;
  } outputs;

  struct parameters
  {
    halp::knob_i32<"Num Channels", halp::range{1, 64, 2}> n_channels;
    halp::knob_i32<"Time Lag (samples)", halp::range{1, 300, 10}> time_lag;
    halp::knob_i32<"Num Dimensions", halp::range{1, 8, 2}> n_dims;
    halp::knob_i32<"Epoch Size (samples)", halp::range{2, 1000, 256}> epoch_size;
    halp::toggle<"Collect Data", halp::toggle_setup{true}> collect;
    halp::impulse_button<"Reset Model"> reset;
  } params;

  VAMPAvnd();
  void operator()();

private:
  void refit_model();
  void reset_state();
  std::unique_ptr<algorithms::VampModel> m_model;
  xt::xarray<double> m_current_epoch;
  std::vector<xt::xarray<double>> m_buffer;
};

}
