/*
 * MinMaxScaler.cpp
 *
 * Adapté de :
 * Plaquette (c) 2015 Sofian Audry        :: info(@)sofianaudry(.)com
 *            Thomas O Fredericks :: tof(@)t-o-f(.)info
 *
 * Adaptation par Luana Belinsky 2025
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "MinMaxScaler.h"
#include "EMA.h"

#include <algorithm>
#include <cmath>
#include <limits>

// ---- Constructors ----- //

MinMaxScaler::MinMaxScaler()
  : _infinite(true)
  , _tau_s(1.0)
  , _minValue(0.0f)
  , _maxValue(0.0f)
  , _smoothedMinValue(0.5f)
  , _smoothedMaxValue(0.5f)
  , _value(0.5f)
  , _n(0)
{
  init_states();
}

MinMaxScaler::MinMaxScaler(double timeWindowSeconds)
  : _infinite(timeWindowSeconds <= 0.0)
  , _tau_s(_infinite ? 1.0 : timeWindowSeconds)
  , _minValue(0.0f)
  , _maxValue(0.0f)
  , _smoothedMinValue(0.5f)
  , _smoothedMaxValue(0.5f)
  , _value(0.5f)
  , _n(0)
{
  init_states();
}

// ---- Time window / decay control ------ //

void MinMaxScaler::timeWindow(double seconds)
{
  bool was_infinite = _infinite;

  _infinite = (seconds <= 0.0);
  _tau_s    = _infinite ? 1.0 : seconds;

  // Switching from finite -> infinite: forget EMA state and reseed on next sample.
  if (!was_infinite && _infinite) {
    _n = 0;
    init_states();
  }
}

double MinMaxScaler::timeWindow() const
{
  return _infinite ? 0.0 : _tau_s;
}

bool MinMaxScaler::timeWindowIsInfinite() const
{
  return _infinite;
}

// ---- Reset ----- //

void MinMaxScaler::reset()
{
  _n = 0;
  init_states();
}

// ---- Main entry ------ //

float MinMaxScaler::put(float x, double dt_seconds)
{
  // First sample: initialize everything from data.
  if (_n == 0) {
    _minValue         = x;
    _maxValue         = x;
    _smoothedMinValue = x;
    _smoothedMaxValue = x;
    _value            = 0.5f; // by convention when min == max
    _n = 1;
    return _value;
  }

  // Update raw min/max with new sample.
  if (x < _minValue) _minValue = x;
  if (x > _maxValue) _maxValue = x;

  // Compute EMA alpha based on window and dt.
  const float alpha = ema_alpha(_infinite, _tau_s, _n, dt_seconds);

  // In finite-window mode, apply decay to min/max toward current sample.
  if (!_infinite) {
    ema_apply_update(_minValue, x, alpha);
    ema_apply_update(_maxValue, x, alpha);
  }

  // Smooth out reported min/max values.
  ema_apply_update(_smoothedMinValue, _minValue,  alpha);
  ema_apply_update(_smoothedMaxValue, _maxValue,  alpha);

  if (_n < std::numeric_limits<std::uint32_t>::max())
    ++_n;

  // Scale current sample into [0, 1] using smoothed min/max.
  const float lo = _smoothedMinValue;
  const float hi = _smoothedMaxValue;

  if (hi <= lo) {
    // Degenerate case: avoid division by zero; keep center at 0.5.
    _value = 0.5f;
  } else {
    float t = (x - lo) / (hi - lo);
    if (t < 0.0f)      t = 0.0f;
    else if (t > 1.0f) t = 1.0f;
    _value = t;
  }

  return _value;
}

// ---- Helpers ----------------------------------------------------------------

// Initialize the internal state
void MinMaxScaler::init_states()
{
  // Start with no valid min/max; they will be initialized on first sample.
  _minValue         =  std::numeric_limits<float>::max();
  _maxValue         = -std::numeric_limits<float>::max();
  _smoothedMinValue = 0.5f;
  _smoothedMaxValue = 0.5f;
  _value            = 0.5f;
}