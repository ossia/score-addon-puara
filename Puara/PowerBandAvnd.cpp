#include "PowerBandAvnd.hpp"

#include <xtensor/containers/xadapt.hpp>
#include <xtensor/views/xview.hpp>
#include <vector>

namespace puara_gestures::objects
{

void PowerBandAvnd::operator()()
{
  const auto& psd_vec = inputs.psd.value;
  const auto& freq_vec = inputs.frequencies.value;

  if (psd_vec.empty() || freq_vec.empty() || psd_vec.size() != freq_vec.size())
  {
    outputs.power.value = 0.0;
    return;
  }

  const double f_min = inputs.f_min;
  const double f_max = inputs.f_max;
  const auto power_type = inputs.power_type.value;

  auto psd_arr = xt::adapt(psd_vec);
  auto freq_arr = xt::adapt(freq_vec);

  std::vector<size_t> valid_indices;
  for (size_t i = 0; i < freq_arr.size(); ++i)
  {
    if (freq_arr(i) >= f_min && freq_arr(i) <= f_max)
    {
      valid_indices.push_back(i);
    }
  }

  if (valid_indices.empty())
  {
    outputs.power.value = 0.0;
    return;
  }

  auto selected_psd = xt::view(psd_arr, xt::keep(valid_indices));

  double band_power = xt::sum(selected_psd)();

  if (power_type == PowerType::Relative)
  {
    double total_power = xt::sum(psd_arr)();
    if (total_power > 0)
    {
      band_power /= total_power;
    }
    else
    {
      band_power = 0.0;
    }
  }

  outputs.power.value = band_power;
}

}
