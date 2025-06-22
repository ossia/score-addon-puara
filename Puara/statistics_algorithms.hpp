#pragma once

#include <boost/math/distributions/students_t.hpp>

#include <xtensor/containers/xarray.hpp>
#include <xtensor/core/xmath.hpp>

#include <utility>

namespace puara_gestures::algorithms
{

inline std::pair<double, double>
calculate_pearson(const xt::xarray<double>& x, const xt::xarray<double>& y)
{
  const size_t n = x.size();
  if(n < 3)
  {
    return {0.0, 1.0};
  }

  const double mean_x = xt::mean(x)();
  const double mean_y = xt::mean(y)();

  const double cov_xy = xt::mean((x - mean_x) * (y - mean_y))();
  const double stddev_x = xt::stddev(x)();
  const double stddev_y = xt::stddev(y)();

  if(stddev_x == 0.0 || stddev_y == 0.0)
  {

    return {0.0, 1.0};
  }
  const double r = cov_xy / (stddev_x * stddev_y);

  const double r_clamped = std::max(-1.0, std::min(1.0, r));

  if(std::abs(r_clamped) == 1.0)
  {

    return {r_clamped, 0.0};
  }
  const double t_stat = r_clamped * std::sqrt((n - 2.0) / (1.0 - r_clamped * r_clamped));

  boost::math::students_t dist(n - 2.0);
  double p = boost::math::cdf(boost::math::complement(dist, std::abs(t_stat))) * 2.0;

  return {r, p};
}

}

#include <xtensor/views/xview.hpp>

#include <vector>

namespace puara_gestures::algorithms
{

enum class PowerBandType
{
  Absolute,
  Relative
};

inline double calculate_power_in_band(
    const xt::xarray<double>& psd, const xt::xarray<double>& freqs, double f_min,
    double f_max, PowerBandType power_type)
{

  std::vector<size_t> valid_indices;
  for(size_t i = 0; i < freqs.size(); ++i)
  {
    if(freqs(i) >= f_min && freqs(i) <= f_max)
    {
      valid_indices.push_back(i);
    }
  }

  if(valid_indices.empty())
  {
    return 0.0;
  }

  auto selected_psd = xt::view(psd, xt::keep(valid_indices));

  double band_power = xt::sum(selected_psd)();

  if(power_type == PowerBandType::Relative)
  {
    double total_power = xt::sum(psd)();
    return (total_power > 0) ? (band_power / total_power) : 0.0;
  }

  return band_power;
}

}
