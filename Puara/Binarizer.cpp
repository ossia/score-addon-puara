#include "Binarizer.hpp"

#include <xtensor/containers/xadapt.hpp>
#include <xtensor/containers/xarray.hpp>
#include <xtensor/core/xfunction.hpp>
#include <xtensor/core/xmath.hpp>

namespace puara_gestures::objects
{

void Binarizer::operator()()
{
  const auto& in_vec = inputs.input_array.value;
  if(in_vec.empty())
  {
    outputs.output_array.value.clear();
    return;
  }
  const float thresh = inputs.threshold;
  const auto thresh_type = inputs.threshold_type.value;

  std::vector<size_t> shape = {in_vec.size()};
  auto in_arr = xt::adapt(in_vec.data(), in_vec.size(), xt::no_ownership(), shape);

  xt::xarray<double> binarized_arr;
  switch(thresh_type)
  {
    case ThresholdType::Above:
      binarized_arr = xt::where(in_arr > thresh, 1.0, 0.0);
      break;
    case ThresholdType::Below:
      binarized_arr = xt::where(in_arr < thresh, 1.0, 0.0);
      break;
    case ThresholdType::Both:
    default:
      binarized_arr = xt::where(xt::abs(in_arr) > thresh, 1.0, 0.0);
      break;
  }
  outputs.output_array.value.assign(binarized_arr.begin(), binarized_arr.end());
}
}
