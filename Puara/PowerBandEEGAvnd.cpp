#include "PowerBandEEGAvnd.hpp"

#include "statistics_algorithms.hpp" // Where our helper function lives

#include <xtensor.hpp>
namespace puara_gestures::objects
{

void PowerBandEEGAvnd::operator()()
{
  const auto& psd_vec = inputs.psd.value;
  const auto& freq_vec = inputs.frequencies.value;

  if(psd_vec.empty() || freq_vec.empty() || psd_vec.size() != freq_vec.size())
  {

    outputs.delta.value = 0.0;
    outputs.theta.value = 0.0;
    outputs.alpha.value = 0.0;
    outputs.low_beta.value = 0.0;
    outputs.high_beta.value = 0.0;
    outputs.gamma.value = 0.0;
    return;
  }

  const auto power_type = inputs.power_type.value;

  auto psd_arr = xt::adapt(psd_vec);
  auto freq_arr = xt::adapt(freq_vec);

  outputs.delta.value
      = algorithms::calculate_power_in_band(psd_arr, freq_arr, 1.0, 3.0, power_type);
  outputs.theta.value
      = algorithms::calculate_power_in_band(psd_arr, freq_arr, 3.0, 7.0, power_type);
  outputs.alpha.value
      = algorithms::calculate_power_in_band(psd_arr, freq_arr, 7.0, 12.0, power_type);
  outputs.low_beta.value
      = algorithms::calculate_power_in_band(psd_arr, freq_arr, 12.0, 20.0, power_type);
  outputs.high_beta.value
      = algorithms::calculate_power_in_band(psd_arr, freq_arr, 20.0, 30.0, power_type);
  outputs.gamma.value
      = algorithms::calculate_power_in_band(psd_arr, freq_arr, 30.0, 50.0, power_type);
}

}
