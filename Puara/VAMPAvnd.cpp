#include "VAMPAvnd.hpp"

namespace puara_gestures::objects
{

VAMPAvnd::VAMPAvnd()
{
  m_current_epoch = xt::xarray<double>::from_shape({0, 0});
  refit_model();
}

void VAMPAvnd::operator()()
{
  if(inputs.reset.value.has_value())
  {
    reset_state();
  }

  if(inputs.time_lag.value != m_model->lag() || inputs.n_dims.value != m_model->dims())
  {
    refit_model();
  }

  const auto& input_vec = inputs.data.value;
  const int n_channels = inputs.n_channels.value;

  if(input_vec.empty() || input_vec.size() % n_channels != 0)
  {
    outputs.comps.value.clear();
    return;
  }

  size_t n_samples = input_vec.size() / n_channels;
  auto input_arr = xt::adapt(
      input_vec.data(), input_vec.size(), xt::no_ownership(),
      std::vector<size_t>{n_samples, (size_t)n_channels});

  if(inputs.collect.value)
  {
    if(m_current_epoch.size() == 0 || m_current_epoch.dimension() == 0)
    {
      m_current_epoch = input_arr;
    }
    else
    {
      // Ensure both arrays have the same number of dimensions and channels
      if(m_current_epoch.shape()[1] == input_arr.shape()[1])
      {
        m_current_epoch = xt::concatenate(xt::xtuple(m_current_epoch, input_arr), 0);
      }
      else
      {
        // If channel count mismatch, restart with new data
        m_current_epoch = input_arr;
      }
    }

    const int target_epoch_size = inputs.epoch_size.value + inputs.time_lag.value;
    if(m_current_epoch.shape()[0] >= target_epoch_size)
    {
      // Take only the required amount of data
      auto epoch_data
          = xt::view(m_current_epoch, xt::range(0, target_epoch_size), xt::all());
      xt::xarray<double> epoch_copy = epoch_data;

      m_buffer.push_back(epoch_copy);
      m_model->partial_fit(epoch_copy);

      // Only solve if we have enough data
      if(m_buffer.size() >= 2) // Need at least 2 epochs for meaningful VAMP
      {
        m_model->solve();
      }
      if(m_current_epoch.shape()[0] > target_epoch_size)
      {
        auto remaining = xt::view(
            m_current_epoch, xt::range(target_epoch_size, m_current_epoch.shape()[0]),
            xt::all());
        m_current_epoch = remaining;
      }
      else
      {
        m_current_epoch = xt::xarray<double>::from_shape({0, (size_t)n_channels});
      }
    }
  }

  if(m_model->is_fitted())
  {
    auto comps_arr = m_model->transform(input_arr);
    if(comps_arr.size() > 0)
    {
      outputs.comps.value.assign(comps_arr.begin(), comps_arr.end());
    }
    else
    {
      outputs.comps.value.clear();
    }
  }
  else
  {
    outputs.comps.value.assign(n_samples * inputs.n_dims.value, 0.0);
  }
}

void VAMPAvnd::refit_model()
{
  m_model = std::make_unique<algorithms::VampModel>(
      inputs.time_lag.value, inputs.n_dims.value);
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
  const int n_channels = inputs.n_channels.value;
  m_current_epoch = xt::xarray<double>::from_shape({0, (size_t)n_channels});
  m_model->reset();
}

}
