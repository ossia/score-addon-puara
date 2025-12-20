#pragma once

#include <halp/controls.hpp>
#include <halp/meta.hpp>

#include <vector>

namespace puara_gestures::objects
{

class TuningReductionAvnd
{
public:
  halp_meta(name, "Tuning Reduction")
  halp_meta(category, "Analysis/Puara")
  halp_meta(c_name, "puara_tuningreduction_avnd")
  halp_meta(description, "Reduces a set of tuning ratios to a smaller musical mode.")
  halp_meta(manual_url, "https://github.com/dav0dea/goofi-pipe")
  halp_meta(uuid, "c3832aa5-afe6-4311-a5b1-4d67684f464c")

  enum class MetricFunction
  {
    Harmsim,
    Consonance,
    Denom
  };

  struct ins
  {
    halp::val_port<"Tuning Ratios", std::vector<double>> tuning;
    halp::knob_i32<"Num Steps", halp::range{2, 20, 5}> n_steps;
    halp::enum_t<MetricFunction, "Function"> function{MetricFunction::Harmsim};
  } inputs;

  struct outs
  {
    halp::val_port<"Reduced Mode", std::vector<double>> reduced;
  } outputs;

  void operator()();
};

}
