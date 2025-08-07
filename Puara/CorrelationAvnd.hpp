#pragma once
#include <halp/controls.hpp>
#include <halp/meta.hpp>
#include <vector>

namespace puara_gestures::objects
{

class CorrelationAvnd
{
public:
  halp_meta(name, "Correlation")
  halp_meta(category, "Analysis/Puara")
  halp_meta(c_name, "puara_correlation_avnd")
  halp_meta(
      description,
      "Computes the Pearson correlation and p-value between two data arrays.")
  halp_meta(manual_url, "https://github.com/dav0dea/goofi-pipe")
  halp_meta(uuid, "202a5150-0452-403f-930e-7fa18564c863")

  struct ins
  {
    halp::val_port<"Data 1", std::vector<double>> data1;
    halp::val_port<"Data 2", std::vector<double>> data2;
  } inputs;

  struct outs
  {
    halp::val_port<"Pearson (r)", double> pearson;
    halp::val_port<"p-value", double> p_value;
  } outputs;

  void operator()();
};

}
