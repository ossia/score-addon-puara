#include "ERPAvnd.hpp"

#include <xtensor/containers/xadapt.hpp>
#include <xtensor/core/xmath.hpp>
#include <xtensor/views/xview.hpp>

namespace puara_gestures::objects
{

void ERPAvnd::operator()()
{
  if(!inputs.reset.value.has_value())
  {
    reset_state();
  }

  const auto& signal_chunk = inputs.signal.value;
  if(signal_chunk.empty())
  {
    return;
  }
  const bool triggered = inputs.trigger.value.has_value();
  if(triggered)
  {
    if(!m_is_collecting || !inputs.delay_retrigger.value)
    {
      m_is_collecting = true;
      m_collected_chunk.clear();
    }
  }
  if(!m_is_collecting)
  {
    return;
  }
  m_collected_chunk.insert(
      m_collected_chunk.end(), signal_chunk.begin(), signal_chunk.end());

  const int target_samples = static_cast<int>(inputs.duration.value * m_sampling_rate);
  if(m_collected_chunk.size() < target_samples)
  {
    return;
  }
  auto collected_arr = xt::adapt(m_collected_chunk);
  auto erp_segment = xt::view(collected_arr, xt::range(0, target_samples));

  if(inputs.baseline.value == BaselineCorrection::Mean)
  {
    erp_segment = erp_segment - xt::mean(erp_segment)();
  }

  if(m_erp_count == 0)
  {
    m_erp_sum = erp_segment;
  }
  else if(m_erp_sum.size() != erp_segment.size())
  {
    reset_state();
    m_erp_sum = erp_segment;
  }
  else
  {
    m_erp_sum += erp_segment;
  }
  m_erp_count++;
  xt::xarray<double> final_erp = m_erp_sum / m_erp_count;
  outputs.erp.value.assign(final_erp.begin(), final_erp.end());
  m_is_collecting = false;
  m_collected_chunk.clear();
}

void ERPAvnd::reset_state()
{
  m_erp_count = 0;
  m_is_collecting = false;
  m_collected_chunk.clear();
  m_erp_sum.resize({0});
  outputs.erp.value.clear();
}

}
