#pragma once

#include <xtensor/xarray.hpp>

#include <functional>
#include <numeric>
#include <utility>

namespace puara_gestures::algorithms
{

struct TuningMatrixResult
{
  xt::xarray<double> matrix;
  xt::xarray<double> metric_per_step;
  double metric_total;
};

inline std::pair<long, long>
double_to_fraction(double value, long max_denominator = 1000)
{
  long p0 = 0, q0 = 1, p1 = 1, q1 = 0;
  double g = value;
  long a, q2;
  while(true)
  {
    a = static_cast<long>(g);
    q2 = q0 + a * q1;
    if(q2 > max_denominator)
      break;
    p0 = p1;
    q0 = q1;
    p1 = p0 + a * p1;
    q1 = q2;
    if(g == static_cast<double>(a))
      break;
    g = 1.0 / (g - static_cast<double>(a));
  }
  return {p1, q1};
}

inline double metric_denom_impl(double ratio)
{
  return static_cast<double>(double_to_fraction(ratio).second);
}

inline double dyad_similarity_impl(double ratio)
{
  auto [p, q] = double_to_fraction(ratio);
  return 1.0 / static_cast<double>(p + q);
}

inline double consonance_impl(double ratio)
{
  auto [p, q] = double_to_fraction(ratio);
  return 1.0 / static_cast<double>(p * q);
}

inline TuningMatrixResult compute_tuning_matrix(
    const xt::xarray<double>& tuning, const std::function<double(double)>& metric_fn)
{
  TuningMatrixResult result;
  size_t n = tuning.size();
  if(n == 0)
    return result;

  result.metric_per_step = xt::zeros<double>({n});
  for(size_t i = 0; i < n; ++i)
  {
    result.metric_per_step(i) = metric_fn(tuning(i));
  }
  result.metric_total = xt::sum(result.metric_per_step)();
  result.matrix = xt::zeros<double>({n, n});
  for(size_t i = 0; i < n; ++i)
  {
    for(size_t j = 0; j < n; ++j)
    {
      result.matrix(i, j) = metric_fn(tuning(j) / tuning(i));
    }
  }
  return result;
}

}

namespace puara_gestures::algorithms
{

inline std::vector<double> reduce_tuning(
    const std::vector<double>& tuning, int n_steps,
    const std::function<double(double)>& metric_fn, bool higher_is_better = true)
{
  if(n_steps <= 0 || tuning.empty())
  {
    return {};
  }

  if(n_steps >= tuning.size())
  {
    auto sorted_tuning = tuning;
    std::sort(sorted_tuning.begin(), sorted_tuning.end());
    return sorted_tuning;
  }
  std::vector<std::pair<double, double>> scored_ratios;
  for(double ratio : tuning)
  {
    scored_ratios.push_back({metric_fn(ratio), ratio});
  }

  std::sort(
      scored_ratios.begin(), scored_ratios.end(), [&](const auto& a, const auto& b) {
    return higher_is_better ? (a.first > b.first) : (a.first < b.first);
  });

  scored_ratios.resize(n_steps);

  std::vector<double> reduced_mode;
  for(const auto& pair : scored_ratios)
  {
    reduced_mode.push_back(pair.second);
  }
  std::sort(reduced_mode.begin(), reduced_mode.end());

  return reduced_mode;
}

}
