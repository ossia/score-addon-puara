/*
 * QuantileScaler.cpp
 *
 * (c) 2025 Sofian Audry        :: info(@)sofianaudry(.)com
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
 *
 * Adaptive quantile-based scaler using Robbins–Monro updates.
 */

#include "QuantileScaler.h"
#include "EMA.h"
#include "helpers.h"  

#include <algorithm>
#include <cmath>
#include <limits>

// Minimum quantile level to avoid ill-defined zero quantile.
static constexpr float kMinimumQuantileLevel = 1e-4f;
static constexpr float kMaximumQuantileLevel = 0.5f;

// Number of standard deviations to cover the full range.
static constexpr float kStddevToRange = 6.0f;

// ---- Span / quantile mapping helpers ------------------------------------ //

float QuantileScaler::lowQuantileLevelToSpan(float level)
{
  // Clamp level into allowed range.
  level = std::clamp(level, kMinimumQuantileLevel, kMaximumQuantileLevel);
  // For symmetric coverage, span ≈ 1 - 2*lowLevel.
  float span = 1.0f - 2.0f * level;
  return std::clamp(span, 0.0f, 1.0f);
}

float QuantileScaler::spanToLowQuantileLevel(float span)
{
  // Clamp span into [0, 1].
  span = std::clamp(span, 0.0f, 1.0f);
  // Inverse of span ≈ 1 - 2*lowLevel -> lowLevel ≈ (1 - span)/2.
  float level = 0.5f * (1.0f - span);
  // Clamp into [kMinimumQuantileLevel, kMaximumQuantileLevel].
  return std::clamp(level, kMinimumQuantileLevel, kMaximumQuantileLevel);
}

// ---- Constructors -------------------------------------------------------- //

QuantileScaler::QuantileScaler()
  : _infinite(true)
  , _tau_s(1.0)
  , _lowQuantileLevel(spanToLowQuantileLevel(kDefaultSpan))
  , _lowQuantile(0.0f)
  , _highQuantile(0.0f)
  , _stddev(0.0f)
  , _value(0.5f)
  , _n(0)
{
  init_states();
}

QuantileScaler::QuantileScaler(double timeWindowSeconds, float span)
  : _infinite(timeWindowSeconds <= 0.0)
  , _tau_s(_infinite ? 1.0 : timeWindowSeconds)
  , _lowQuantileLevel(spanToLowQuantileLevel(span))
  , _lowQuantile(0.0f)
  , _highQuantile(0.0f)
  , _stddev(0.0f)
  , _value(0.5f)
  , _n(0)
{
  init_states();
}

// ---- Time window control ------------------------------------------------- //

void QuantileScaler::timeWindow(double seconds)
{
  bool was_infinite = _infinite;

  _infinite = (seconds <= 0.0);
  _tau_s    = _infinite ? 1.0 : seconds;

  // Switching from finite -> infinite: drop old EMA state and reseed on next sample.
  if (!was_infinite && _infinite) {
    _n = 0;
    init_states();
  }
}


double QuantileScaler::timeWindow() const
{
  return _infinite ? 0.0 : _tau_s;
}

bool QuantileScaler::timeWindowIsInfinite() const
{
  return _infinite;
}

// ---- Reset / init -------------------------------------------------------- //

void QuantileScaler::reset()
{
  _n = 0;
  init_states();
}

void QuantileScaler::init_states()
{
  _lowQuantile  =  std::numeric_limits<float>::max();
  _highQuantile = -std::numeric_limits<float>::max();
  _stddev       = 0.0f;
  _value        = 0.5f;
}

// ---- Span / quantiles API ------------------------------------------------ //

void QuantileScaler::span(float span)
{
  _lowQuantileLevel = spanToLowQuantileLevel(span);
}

float QuantileScaler::span() const
{
  return lowQuantileLevelToSpan(_lowQuantileLevel);
}

void QuantileScaler::lowQuantileLevel(float level)
{
  level = std::clamp(level, 0.0f, kMaximumQuantileLevel);
  _lowQuantileLevel = std::clamp(level, kMinimumQuantileLevel, kMaximumQuantileLevel);
}

void QuantileScaler::highQuantileLevel(float level)
{
  // low + high = 1 for symmetric coverage.
  lowQuantileLevel(1.0f - level);
}

// ---- Main entry ---------------------------------------------------------- //

float QuantileScaler::put(float x, double dt_seconds)
{
  // First sample: initialize quantiles and stddev.
  if (_n == 0) {
    _lowQuantile  = x;
    _highQuantile = x;
    _stddev       = 0.0f;
    _value        = 0.5f;
    _n = 1;
    return _value;
  }

  // Compute EMA alpha based on window and dt.
  const float alpha = ema_alpha(_infinite, _tau_s, _n, dt_seconds);

  // Mid-quantile and deviation for stddev estimate.
  const float midQuantile = 0.5f * (_lowQuantile + _highQuantile);
  const float deviation   = std::fabs(x - midQuantile);

  if (_n == 1 && _stddev == 0.0f) {
    _stddev = deviation;
  } else {
    ema_apply_update(_stddev, deviation, alpha);
  }

  // Compute Robbins–Monro step size scaled by stddev.
  float eta = alpha * kStddevToRange * _stddev;
  if (eta < 0.0f) eta = 0.0f;

  const float etaLevel = eta * _lowQuantileLevel;

  // Update quantiles depending on where x lies.
  if (x <= _lowQuantile) {
    // Smaller than both quantiles.
    _lowQuantile  -= eta - etaLevel; // decrease
    _highQuantile -= etaLevel;       // decrease
    // Prevent overshooting.
    _lowQuantile  = std::max(_lowQuantile,  x);
    _highQuantile = std::max(_highQuantile, x);
  }
  else if (x <= _highQuantile) {
    // Between low and high.
    _lowQuantile  += etaLevel;       // increase
    _highQuantile -= etaLevel;       // decrease
    // Prevent overshooting.
    _lowQuantile  = std::min(_lowQuantile,  x);
    _highQuantile = std::max(_highQuantile, x);
  }
  else {
    // Larger than both quantiles.
    _lowQuantile  += etaLevel;        // increase
    _highQuantile += eta - etaLevel;  // increase
    // Prevent overshooting.
    _lowQuantile  = std::min(_lowQuantile,  x);
    _highQuantile = std::min(_highQuantile, x);
  }

  // Optional decay of quantiles toward their mid-point in finite-window mode.
  if (!_infinite) {
    ema_apply_update(_lowQuantile,  midQuantile, alpha);
    ema_apply_update(_highQuantile, midQuantile, alpha);
  }

  // Clamp quantiles to avoid inversions.
  if (_lowQuantile > _highQuantile) {
    const float mid = 0.5f * (_lowQuantile + _highQuantile);
    _lowQuantile  = mid;
    _highQuantile = mid;
  }

  if (_n < std::numeric_limits<std::uint32_t>::max())
    ++_n;

  // Compute rescaled value using shared map helper into [0, 1].
  _value = helpers::map(x, _lowQuantile, _highQuantile, 0.0f, 1.0f);
  return _value;
}
