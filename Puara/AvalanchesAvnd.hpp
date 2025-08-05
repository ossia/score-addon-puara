#pragma once

#include <halp/audio.hpp>
#include <halp/controls.hpp>
#include <halp/meta.hpp>

#include <vector>

namespace puara_gestures::objects
{

class AvalanchesAvnd
{
public:
  halp_meta(name, "Avalanches")
  halp_meta(category, "Analysis/Data")
  halp_meta(c_name, "puara_avalanches_avnd")
  halp_meta(description, "Detects avalanches in a stream of binarized events.")
  halp_meta(manual_url, "https://github.com/dav0dea/goofi-pipe")
  halp_meta(uuid, "268579f9-0848-4d31-9937-4904445fcf2e")

  struct ins
  {
    halp::val_port<"Input", std::vector<double>> input_array;
    halp::knob_f32<"Time Bin (s)", halp::range{0.0, 0.05, 0.008}> time_bin;
  } inputs;

  struct outs
  {
    halp_meta(name, "Size")
    halp::val_port<"Size", std::vector<double>> size;
    halp::val_port<"Duration (s)", std::vector<double>> duration;
  } outputs;

  void prepare(halp::setup& setup) { m_sampling_rate = setup.rate; }

  void operator()();

private:
  double m_sampling_rate{1000.0};
};

}
