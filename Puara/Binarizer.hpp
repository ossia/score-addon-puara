#pragma once

#include <halp/controls.hpp>
#include <halp/meta.hpp>

#include <vector>

namespace puara_gestures::objects
{

class Binarizer
{
public:
  halp_meta(name, "Binarize")
  halp_meta(category, "Analysis/Puara")
  halp_meta(c_name, "puara_binarize_avnd")
  halp_meta(description, "Binarizes an input array based on a threshold.")
  halp_meta(manual_url, "https://github.com/dav0dea/goofi-pipe")
  halp_meta(uuid, "84931c47-894b-45bb-addc-3163ae3593da")

  enum class ThresholdType
  {
    Both,
    Above,
    Below
  };
  struct
  {

    halp::val_port<"Input", std::vector<double>> input_array;
    halp::knob_f32<"Threshold", halp::range{0.0, 5.0, 2.0}> threshold;
    halp::enum_t<ThresholdType, "Threshold Type"> threshold_type{ThresholdType::Both};

  } inputs;

  struct
  {
    halp::val_port<"Binarized Output", std::vector<double>> output_array;
  } outputs;

  void operator()();
};

}
