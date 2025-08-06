#include "CompassAvnd.hpp"

#include <xtensor/containers/xadapt.hpp>
#include <xtensor/containers/xarray.hpp>

#include <cmath>

#include <numbers>

namespace puara_gestures::objects
{

void CompassAvnd::operator()()
{
  const auto& pole1_vec = inputs.pole1.value;
  const auto& pole2_vec = inputs.pole2.value;

  outputs.angles.value.clear();

  if(pole1_vec.empty() || pole2_vec.empty() || pole1_vec.size() != pole2_vec.size())
  {

    return;
  }

  auto pole1_arr = xt::adapt(pole1_vec);
  auto pole2_arr = xt::adapt(pole2_vec);

  xt::xarray<double> vector_diff = pole2_arr - pole1_arr;

  if(vector_diff.size() < 2)
  {
    return;
  }
  for(size_t i = 0; i < vector_diff.size() - 1; ++i)
  {
    const double x = vector_diff(i);
    const double y = vector_diff(i + 1);
    const double angle_rad = std::atan2(y, x);
    const double angle_deg = angle_rad * 180.0 / std::numbers::pi;
    double normalized_deg = std::fmod(angle_deg, 360.0);
    if(normalized_deg < 0)
    {
      normalized_deg += 360.0;
    }

    outputs.angles.value.push_back(normalized_deg);
  }
}

}
