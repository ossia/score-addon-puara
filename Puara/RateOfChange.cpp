#include "RateOfChange.hpp"

#include <algorithm>
#include <cmath> // std::isfinite

namespace puara_gestures::objects
{

// tiny guard for denominator: avoid division by zero
static constexpr double kDtEps = 1e-12;

//==============Ring-buffer helpers==================//
// returns the nth newest sample, n = 0 is newest
const RateOfChange::Sample&
RateOfChange::nth_newest_sample(std::size_t n) const
{
  // logical order: oldest = _buf[_head], newest = _buf[(_head + _size - 1) % _physCap]
  // nth newest index = newest_index - n
  const std::size_t pos = (_head + _size - 1 - n) % _physCap;
  return _buf[pos];
}

// drop the oldest sample from the window
void RateOfChange::drop_oldest_sample()
{
  if (_size <= 1)
    return; // nothing to drop or only one sample; keep at least the oldest

  std::size_t new_head = (_head + 1) % _physCap;

  // new oldest is at new_head, so its dt becomes excluded from denominator
  _sumDt -= _buf[new_head].dt;

  _head = new_head;
  --_size;
}

//==================Push sample into buffer=============================//
// push a new sample into the ring buffer
void RateOfChange::push(float x, double dt)
{
  // first sample: its dt does not contribute to the denominator
  if (_size == 0)
  {
    _buf[0] = {x, 0.0};
    _size   = 1;
    return;
  }

  // if physically full, drop oldest sample before writing
  if (_size == _physCap)
  {
    drop_oldest_sample();
  }

  // append at tail (newest sample)
  const std::size_t tail = (_head + _size) % _physCap;
  _buf[tail] = {x, dt};
  ++_size;

  // dt of newly added sample contributes to sum of dt 
  _sumDt += dt;
}

//=============Window policy: decides which samples stay==========//
// enforce sample window: keep at most N newest samples
void RateOfChange::apply_window_policy()
{
  if (_cap < 2)
    _cap = 2;

    while (_size > _cap)
    {
      drop_oldest_sample();
    }
}

//=================Parameter watcher==============//
void RateOfChange::update_window_from_params()
{
  int   sample_window_v = inputs.sample_window.value;
  if (sample_window_size.changed(sample_window_v))
  {
    if (sample_window_v < 2)
    sample_window_v = 2;

  // logical limit (how many newest samples we are allowed to keep)
  _cap = sample_window_v;
  }
}

//===========Derivative===============//
// derivative across the current window: (x_last - x_first) / sum(dt)
// "base" is in units of per-second; unit scaling is applied afterwards
float RateOfChange::delta_per(int unit) const
{
  if (_size < 2 || !(_sumDt > kDtEps))       
    return 0.0f;

  const Sample& a = _buf[_head];              // oldest
  const Sample& b = nth_newest_sample(0);     // newest

  const double dx   = static_cast<double>(b.x) - static_cast<double>(a.x);
  const double base = dx / _sumDt;            // per second

  if (!std::isfinite(base))
    return 0.0f;

  float v = static_cast<float>(base);

  switch (unit)
  {
    case PerMillisecond:
      v *= 1.0f / 1000.0f;  // per-ms
      break;
    case PerSecond:
      // as-is
      break;
    case PerMinute:
      v *= 60.0f;           // per-min
      break;
    case PerHour:
      v *= 3600.0f;         // per-hr
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

  _buf.assign(_physCap, Sample{0.0f, 0.0});
  _cap  = static_cast<std::size_t>(inputs.sample_window.value);
  _head = 0;
  _size = 0;
  _sumDt = 0.0;

  // init watcher 
  sample_window_size.last  = inputs.sample_window.value;
  sample_window_size.first = false;
}

void RateOfChange::operator()(halp::tick t)
{
  // update internal window config according to current parameters
  update_window_from_params();

  // compute dt from tick (seconds)
  double dt = 0.0;
  if (setup.rate > 0.0)
  {
    dt = static_cast<double>(t.frames) / setup.rate;
    if (dt < 0.0)
      dt = 0.0;
  }

  const float x = inputs.signal;

  // push new sample
  push(x, dt);

  // enforce window policy
  apply_window_policy();

  // compute derivative in chosen units
  outputs.out  = delta_per(inputs.output_units.value);
}

} // namespace puara_gestures::objects
