#pragma once

#include <xtensor/containers/xarray.hpp>
#include <xtensor/core/xmath.hpp>
#include <boost/math/distributions/students_t.hpp>
#include <utility>

namespace puara_gestures::algorithms
{

// Calculates the Pearson correlation coefficient 'r' and the two-tailed p-value.
inline std::pair<double, double> calculate_pearson(const xt::xarray<double>& x, const xt::xarray<double>& y)
{
  const size_t n = x.size();
  if (n < 3) {
    // Not enough data to compute correlation or p-value meaningfully
    return {0.0, 1.0};
  }

  // Calculate means
  const double mean_x = xt::mean(x)();
  const double mean_y = xt::mean(y)();

  // Calculate covariance and standard deviations
  const double cov_xy = xt::mean((x - mean_x) * (y - mean_y))();
  const double stddev_x = xt::stddev(x)();
  const double stddev_y = xt::stddev(y)();

  // Calculate Pearson's r
  if (stddev_x == 0.0 || stddev_y == 0.0) {
    // If one variable is constant, correlation is not defined.
    return {0.0, 1.0};
  }
  const double r = cov_xy / (stddev_x * stddev_y);

  // Clamp r to avoid domain errors in p-value calculation
  const double r_clamped = std::max(-1.0, std::min(1.0, r));

  // Calculate p-value from the t-statistic
  if (std::abs(r_clamped) == 1.0) {
    // Perfect correlation
    return {r_clamped, 0.0};
  }
  const double t_stat = r_clamped * std::sqrt((n - 2.0) / (1.0 - r_clamped * r_clamped));

  boost::math::students_t dist(n - 2.0);
  double p = boost::math::cdf(boost::math::complement(dist, std::abs(t_stat))) * 2.0;

  return {r, p};
}

}
// Add this to your statistics_algorithms.hpp file

#include <xtensor/views/xview.hpp>
#include <vector>

namespace puara_gestures::algorithms
{
// (The calculate_pearson function is already here)

// Helper enum to match the one in our processors
enum class PowerBandType { Absolute, Relative };

// Refactored function to calculate power in a specific band
inline double calculate_power_in_band(
    const xt::xarray<double>& psd,
    const xt::xarray<double>& freqs,
    double f_min,
    double f_max,
    PowerBandType power_type)
{
  // Find indices of frequencies within the desired band
  std::vector<size_t> valid_indices;
  for (size_t i = 0; i < freqs.size(); ++i)
  {
    if (freqs(i) >= f_min && freqs(i) <= f_max)
    {
      valid_indices.push_back(i);
    }
  }

  if (valid_indices.empty())
  {
    return 0.0;
  }

  // Create a view of the PSD array using only the valid indices
  auto selected_psd = xt::view(psd, xt::keep(valid_indices));

  // Compute the power in the band
  double band_power = xt::sum(selected_psd)();

  // Handle relative power calculation
  if (power_type == PowerBandType::Relative)
  {
    double total_power = xt::sum(psd)();
    return (total_power > 0) ? (band_power / total_power) : 0.0;
  }

  return band_power;
}

}
