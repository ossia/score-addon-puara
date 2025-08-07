#pragma once

#include <halp/controls.hpp>
#include <halp/meta.hpp>

#include <vector>

namespace puara_gestures::objects
{

class TuningMatrixAvnd
{
public:
  halp_meta(name, "Tuning Matrix")
  halp_meta(category, "Analysis/Puara")
  halp_meta(c_name, "puara_tuningmatrix_avnd")
  halp_meta(description, "Creates a similarity matrix from a set of tuning ratios.")
  halp_meta(manual_url, "https://github.com/dav0dea/goofi-pipe")
  halp_meta(uuid, "76207cf7-74e2-46f4-a116-ddac30dbcb16")

  enum class MetricFunction
  {
    DyadSimilarity,
    Consonance,
    MetricDenom
  };

  struct
  {
    halp::val_port<"Tuning Ratios", std::vector<double>> tuning;
    halp::enum_t<MetricFunction, "Function"> function{MetricFunction::DyadSimilarity};
  } inputs;

  struct
  {
    halp::val_port<"Matrix", std::vector<double>> matrix;
    halp::val_port<"Metric per Step", std::vector<double>> metric_per_step;
    halp::val_port<"Overall Metric", double> metric;
  } outputs;

  void operator()();
};

}
