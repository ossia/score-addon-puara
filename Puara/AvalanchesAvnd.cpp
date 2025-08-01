#include "AvalanchesAvnd.hpp"

namespace puara_gestures::objects
{

void AvalanchesAvnd::operator()()
{
  const auto& in_vec = inputs.input_array.value;
  const double time_bin_sec = inputs.time_bin;
  outputs.size.value.clear();
  outputs.duration.value.clear();

  if(in_vec.empty())
  {
    return;
  }
  std::vector<int> event_indices;
  for(int i = 0; i < in_vec.size(); ++i)
  {
    if(in_vec[i] > 0.5)
    {
      event_indices.push_back(i);
    }
  }

  if(event_indices.empty())
  {
    return;
  }

  const int max_iei_samples = static_cast<int>(time_bin_sec * m_sampling_rate);

  int avalanche_start_index = event_indices[0];
  int current_avalanche_size = 1;

  for(size_t i = 1; i < event_indices.size(); ++i)
  {
    const int prev_event_index = event_indices[i - 1];
    const int current_event_index = event_indices[i];
    const int delta_samples = current_event_index - prev_event_index;

    if(delta_samples <= max_iei_samples)
    {
      current_avalanche_size++;
    }
    else
    {
      const int avalanche_end_index = prev_event_index;
      const double duration_samples = avalanche_end_index - avalanche_start_index;
      const double duration_sec = duration_samples / m_sampling_rate;

      outputs.size.value.push_back(current_avalanche_size);
      outputs.duration.value.push_back(duration_sec);
      avalanche_start_index = current_event_index;
      current_avalanche_size = 1;
    }
  }
  const int final_avalanche_end_index = event_indices.back();
  const double final_duration_samples
      = final_avalanche_end_index - avalanche_start_index;
  const double final_duration_sec = final_duration_samples / m_sampling_rate;

  outputs.size.value.push_back(current_avalanche_size);
  outputs.duration.value.push_back(final_duration_sec);
}

}
