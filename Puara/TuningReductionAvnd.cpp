#include "TuningReductionAvnd.hpp"

#include "tuning_algorithms.hpp"

namespace puara_gestures::objects
{

void TuningReductionAvnd::operator()()
{
  const auto& tuning_vec = inputs.tuning.value;
  if(tuning_vec.empty())
  {
    outputs.reduced.value.clear();
    return;
  }

  int n_steps = inputs.n_steps.value;
  std::function<double(double)> metric_fn;
  bool higher_is_better = true;

  switch(inputs.function.value)
  {
    case MetricFunction::Consonance:
      metric_fn = algorithms::consonance_impl;
      break;
    case MetricFunction::Denom:
      metric_fn = algorithms::metric_denom_impl;
      higher_is_better = false;
      break;
    case MetricFunction::Harmsim:
    default:

      metric_fn = algorithms::dyad_similarity_impl;
      break;
  }

  auto reduced_mode
      = algorithms::reduce_tuning(tuning_vec, n_steps, metric_fn, higher_is_better);

  outputs.reduced.value = std::move(reduced_mode);
}

}
