#include "RateOfChange.hpp"

#include <cmath> // std::isfinite

#include <algorithm>

namespace puara_gestures::objects
{

// tiny guard for denominator: avoid division by zero
static constexpr double kDtEps = 1e-12;

//==============Ring-buffer helpers==================//
// returns the nth newest sample, n = 0 is newest
const RateOfChange::Sample& RateOfChange::nth_newest_sample(std::size_t n) const
{
  const std::size_t pos = (_head + _size - 1 - n) % _physCap;
  return _buf[pos];
}

// drop the oldest sample from the window
void RateOfChange::drop_oldest_sample()
{
  if(_size <= 1)
    return; // keep at least one sample for holding value

  std::size_t new_head = (_head + 1) % _physCap;

  // new oldest is at new_head, so its dt becomes excluded from denominator
  _sumDt -= _buf[new_head].dt;
  _sumTime -= _buf[new_head].dt;

  _head = new_head;
  --_size;
}

//==================Push sample into buffer=============================//
// push a new sample into the ring buffer
void RateOfChange::push(float x, double dt)
{
  // Update last value and time
  _lastValue = x;
  _lastValueTime = _totalTime + dt;

  // If physically full, drop oldest (but keep at least 1)
  if(_size == _physCap)
  {
    drop_oldest_sample();
  }

  // Append at tail (newest sample)
  const std::size_t tail = (_head + _size) % _physCap;
  _buf[tail] = {x, dt, _totalTime + dt};
  ++_size;

  // dt of newly added sample contributes to sums
  _sumDt += dt;
  _sumTime += dt;

  // update absolute time
  _totalTime += dt;
}

//=============Window policy: decides which samples stay==========//
// enforce window based on current mode
void RateOfChange::apply_window_policy()
{
  if(inputs.window_mode == WindowMode::SampleCount)
  {
    // Sample count mode - simple
    if(_cap < 2)
      _cap = 2;

    while(_size > _cap)
    {
      drop_oldest_sample();
    }
  }
  else // WindowMode::TimeWindow
  {
    // Time window mode with value holding
    if(_maxTime < kDtEps)
      _maxTime = kDtEps;

    // We always need at least 1 sample (the held value)
    if(_size == 0)
      return;

    // Ensure we have at least 2 samples for rate calculation
    // If we only have 1, duplicate it (creates flat signal)
    if(_size == 1)
    {
      // Duplicate the single sample at current time
      const std::size_t tail = (_head + _size) % _physCap;
      _buf[tail] = {_lastValue, 0.0, _totalTime}; // dt = 0, same time
      ++_size;
      // _sumDt stays 0, _sumTime stays 0
      return;
    }

    // Now we have at least 2 samples

    // Calculate time span from oldest to newest
    double newestTime = nth_newest_sample(0).t;
    double oldestTime = _buf[_head].t;
    double windowSpan = newestTime - oldestTime;

    // If window span exceeds max time, we need to adjust
    if(windowSpan > _maxTime)
    {
      // Option 1: Drop oldest samples until window fits
      // But we must keep at least 2 samples
      while(_size > 2 && windowSpan > _maxTime)
      {
        drop_oldest_sample();

        if(_size >= 2)
        {
          newestTime = nth_newest_sample(0).t;
          oldestTime = _buf[_head].t;
          windowSpan = newestTime - oldestTime;
        }
      }

      // If after dropping we still have too large a window,
      // it means our two samples are too far apart
      if(_size >= 2 && windowSpan > _maxTime)
      {
        // Replace oldest sample with a synthetic sample
        // that's exactly _maxTime seconds before newest
        double targetOldestTime = newestTime - _maxTime;

        // Update oldest sample
        double newDt = newestTime - targetOldestTime;
        _buf[_head] = {_lastValue, newDt, targetOldestTime};

        // Recalculate sums
        _sumDt = newDt; // Only one dt between the two samples
        _sumTime = newDt;
        // windowSpan is now exactly _maxTime
      }
    }
  }
}

//=================Parameter watcher==============//
void RateOfChange::update_window_from_params()
{
  // Check if window mode changed
  if(window_mode_watcher.changed(inputs.window_mode))
  {
    // When switching modes, we need to apply the new policy
    apply_window_policy();
  }

  if(inputs.window_mode == WindowMode::SampleCount)
  {
    // Sample count mode
    int sample_count_v = inputs.sample_count;
    if(sample_count_watcher.changed(sample_count_v))
    {
      if(sample_count_v < 2)
        sample_count_v = 2;

      _cap = static_cast<std::size_t>(sample_count_v);
      apply_window_policy();
    }
  }
  else // WindowMode::TimeWindow
  {
    // Time window mode
    int time_window_ms = static_cast<int>(inputs.time_window * 1000.0f);
    if(time_window_watcher.changed(time_window_ms))
    {
      float time_window_v = static_cast<float>(inputs.time_window);
      if(time_window_v < kDtEps)
        time_window_v = static_cast<float>(kDtEps);

      _maxTime = static_cast<double>(time_window_v);
      apply_window_policy();
    }
  }
}

//===========Derivative===============//
// derivative across the current window: (x_last - x_first) / sum(dt)
// "base" is in units of per-second; unit scaling is applied afterwards
float RateOfChange::delta_per(int unit) const
{
  // Need at least 2 samples to compute rate
  if(_size < 2)
    return 0.0f; // Not enough data → rate = 0

  // Denominator check
  if(!(_sumDt > kDtEps))
  {
    // If dt is 0, rate is either infinite or undefined
    // For held values (identical timestamps), rate should be 0
    const Sample& a = _buf[_head];
    const Sample& b = nth_newest_sample(0);

    if(a.x == b.x)
      return 0.0f; // No change in value
    else
      return 0.0f; // Could also return large number, but 0 is safer
  }

  const Sample& a = _buf[_head];          // oldest
  const Sample& b = nth_newest_sample(0); // newest

  const double dx = static_cast<double>(b.x) - static_cast<double>(a.x);
  const double base = dx / _sumDt; // per second

  if(!std::isfinite(base))
    return 0.0f;

  float v = static_cast<float>(base);

  switch(unit)
  {
    case PerMillisecond:
      v *= 1.0f / 1000.0f; // per-ms
      break;
    case PerSecond:
      // as-is
      break;
    case PerMinute:
      v *= 60.0f; // per-min
      break;
    case PerHour:
      v *= 3600.0f; // per-hr
      break;
    default:
      break;
  }

  return v;
}

//=============Score integration==================//
void RateOfChange::prepare(halp::setup info)
{
  setup = info;

  _buf.assign(_physCap, Sample{0.0f, 0.0, 0.0});

  // Initialize from inputs
  _cap = static_cast<std::size_t>(inputs.sample_count);
  _maxTime = static_cast<double>(inputs.time_window);

  _head = 0;
  _size = 0;
  _sumDt = 0.0;
  _sumTime = 0.0;
  _totalTime = 0.0;
  _lastValue = 0.0f;
  _lastValueTime = 0.0;

  // init watchers
  window_mode_watcher.last = inputs.window_mode;
  window_mode_watcher.first = false;

  sample_count_watcher.last = inputs.sample_count;
  sample_count_watcher.first = false;

  time_window_watcher.last = static_cast<int>(inputs.time_window * 1000.0f);
  time_window_watcher.first = false;

  output_units_watcher.last = inputs.output_units.value;
  output_units_watcher.first = false;
}

void RateOfChange::operator()(halp::tick t)
{
  // update internal window config according to current parameters
  update_window_from_params();

  // compute dt from tick (seconds)
  double dt = 0.0;
  if(setup.rate > 0.0)
  {
    dt = static_cast<double>(t.frames) / setup.rate;
    if(dt < 0.0)
      dt = 0.0;
  }

  const float x = inputs.signal;

  // push new sample (even if value hasn't changed)
  push(x, dt);

  // enforce window policy
  apply_window_policy();

  // compute derivative in chosen units
  outputs.out = delta_per(inputs.output_units.value);

  // update diagnostic outputs
  outputs.actual_samples = static_cast<int>(_size);
  outputs.actual_time = static_cast<float>(_sumTime);
}

} // namespace puara_gestures::objects