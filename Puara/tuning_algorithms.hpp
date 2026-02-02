#pragma once

#include <xtensor/containers/xarray.hpp>
#include <xtensor/core/xmath.hpp>

#include <algorithm>
#include <functional>
#include <numeric>
#include <utility>
#include <vector>

namespace puara_gestures::algorithms
{

struct TuningMatrixResult
{
  xt::xarray<double> matrix;
  xt::xarray<double> metric_per_step;
  double metric_total;
};

/**
 * @brief Converts a double to its nearest simplified fraction (p/q)
 * using a continued fraction algorithm.
 * @param value The floating-point number to convert.
 * @param max_denominator The largest denominator to consider, preventing overly complex fractions.
 * @return A std::pair containing the numerator (p) and denominator (q).
 */
inline std::pair<long, long>
double_to_fraction(double value, long max_denominator = 10000)
{
  long p0 = 0, q0 = 1, p1 = 1, q1 = 0;
  double g = value;
  long a;
  while(true)
  {
    a = static_cast<long>(g);
    long q2 = q0 + a * q1;
    if(q2 > max_denominator)
      break;
    long p2 = p0 + a * p1;
    p0 = p1;
    q0 = q1;
    p1 = p2;
    q1 = q2;
    if(std::abs(g - a) < 1e-9)
      break;
    g = 1.0 / (g - static_cast<double>(a));
  }
  return {p1, q1};
}
inline double metric_denom_impl(double ratio)
{
  if(ratio <= 0)
    return std::numeric_limits<double>::infinity();
  return static_cast<double>(double_to_fraction(ratio).second);
}
inline double dyad_similarity_impl(double ratio)
{
  if(ratio <= 0)
    return 0.0;
  auto [p, q] = double_to_fraction(ratio);
  if(p + q == 0)
    return 0.0;
  return 1.0 / static_cast<double>(p + q);
}
inline double consonance_impl(double ratio)
{
  if(ratio <= 0)
    return 0.0;
  auto [p, q] = double_to_fraction(ratio);
  if(p * q == 0)
    return 0.0;
  return 1.0 / static_cast<double>(p * q);
}

/**
 * @brief C++ implementation of the main `tuning_cons_matrix` logic.
 * Computes a similarity matrix for a given set of tuning ratios.
 * @param tuning The input tuning ratios.
 * @param metric_fn The function used to evaluate the harmonicity of each interval.
 * @return A TuningMatrixResult struct containing the matrix and other metrics.
 */
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

/**
 * @brief C++ implementation of the `create_mode` logic.
 * Reduces a large set of tuning ratios to a smaller, harmonically coherent subset (a mode).
 * @param tuning The input vector of tuning ratios.
 * @param n_steps The desired number of notes in the output mode.
 * @param metric_fn The function to score the "goodness" of each ratio.
 * @param higher_is_better True if a higher score is better (e.g., similarity), false otherwise (e.g., denominator).
 * @return A sorted std::vector containing the reduced musical mode.
 */
inline std::vector<double> reduce_tuning(
    const std::vector<double>& tuning, int n_steps,
    const std::function<double(double)>& metric_fn, bool higher_is_better = true)
{
  if(n_steps <= 0 || tuning.empty())
  {
    return {};
  }
  if(n_steps >= (int)tuning.size())
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
