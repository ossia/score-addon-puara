#include "TuningMatrixAvnd.hpp"

#include "tuning_algorithms.hpp"

#include <xtensor/containers/xadapt.hpp>
#include <xtensor/core/xmath.hpp>

namespace puara_gestures::objects
{

void TuningMatrixAvnd::operator()()
{
  const auto& tuning_vec = inputs.tuning.value;
  if(tuning_vec.empty())
  {
    outputs.matrix.value.clear();
    outputs.metric_per_step.value.clear();
    outputs.metric.value = 0.0;
    return;
  }

  std::function<double(double)> metric_fn;
  switch(inputs.function.value)
  {
    case MetricFunction::Consonance:
      metric_fn = algorithms::consonance_impl;
      break;
    case MetricFunction::MetricDenom:
      metric_fn = algorithms::metric_denom_impl;
      break;
    case MetricFunction::DyadSimilarity:
    default:
      metric_fn = algorithms::dyad_similarity_impl;
      break;
  }

  auto results = algorithms::compute_tuning_matrix(xt::adapt(tuning_vec), metric_fn);

  auto min_val = xt::amin(results.matrix)();
  auto max_val = xt::amax(results.matrix)();
  double range = max_val - min_val;
  xt::xarray<double> scaled_matrix = (range > 1e-9)
                                         ? xt::eval((results.matrix - min_val) / range)
                                         : xt::eval(results.matrix);

  outputs.matrix.value.assign(scaled_matrix.begin(), scaled_matrix.end());
  outputs.metric_per_step.value.assign(
      results.metric_per_step.begin(), results.metric_per_step.end());
  outputs.metric.value = results.metric_total;
}

}
