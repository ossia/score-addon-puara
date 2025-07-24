#include "VAMPAvnd.hpp"

namespace puara_gestures::objects
{

VAMPAvnd::VAMPAvnd()
{
  refit_model();
}

void VAMPAvnd::operator()()
{
  if(!params.reset.value.has_value())
  {
    reset_state();
  }
  if(params.time_lag.value != m_model->lag() || params.n_dims.value != m_model->dims())
  {
    refit_model();
  }

  const auto& input_vec = inputs.data.value;
  const int n_channels = params.n_channels.value;
  if(input_vec.empty() || input_vec.size() % n_channels != 0)
  {
    outputs.comps.value.clear();
    return;
  }
  size_t n_samples = input_vec.size() / n_channels;
  auto input_arr = xt::adapt(
      input_vec.data(), input_vec.size(), xt::no_ownership(),
      std::vector<size_t>{n_samples, (size_t)n_channels});

  if(params.collect.value)
  {
    if(m_current_epoch.shape()[0] == 0)
    {
      m_current_epoch = input_arr;
    }
    else
    {
      m_current_epoch = xt::concatenate(xt::xtuple(m_current_epoch, input_arr), 0);
    }

    const int target_epoch_size = params.epoch_size.value + params.time_lag.value;
    if(m_current_epoch.shape()[0] >= target_epoch_size)
    {
      m_buffer.push_back(m_current_epoch);
      m_model->partial_fit(m_current_epoch);
      m_model->solve();
      m_current_epoch.resize({0, (size_t)n_channels});
    }
  }
  if(m_model->is_fitted())
  {
    auto comps_arr = m_model->transform(input_arr);
    outputs.comps.value.assign(comps_arr.begin(), comps_arr.end());
  }
  else
  {
    outputs.comps.value.clear();
  }
}

void VAMPAvnd::refit_model()
{
  m_model = std::make_unique<algorithms::VampModel>(
      params.time_lag.value, params.n_dims.value);
  for(const auto& epoch : m_buffer)
  {
    m_model->partial_fit(epoch);
  }
  if(!m_buffer.empty())
  {
    m_model->solve();
  }
}

void VAMPAvnd::reset_state()
{
  m_buffer.clear();
  m_current_epoch.resize({0, (size_t)params.n_channels.value});
  m_model->reset();
}

}
