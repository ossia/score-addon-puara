#include "CorrelationAvnd.hpp"

#include "statistics_algorithms.hpp"

#include <xtensor/containers/xadapt.hpp>

namespace puara_gestures::objects
{

void CorrelationAvnd::operator()()
{
  const auto& vec1 = inputs.data1.value;
  const auto& vec2 = inputs.data2.value;

  if(vec1.empty() || vec2.empty() || vec1.size() != vec2.size())
  {
    outputs.pearson.value = 0.0;
    outputs.p_value.value = 1.0;
    return;
  }

  auto arr1 = xt::adapt(vec1);
  auto arr2 = xt::adapt(vec2);
  auto [r, p] = algorithms::calculate_pearson(arr1, arr2);

  outputs.pearson.value = r;
  outputs.p_value.value = p;
}

}
