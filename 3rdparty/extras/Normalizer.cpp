/*
 * Normalizer.cpp
 *
 * Adapté de :
 * Plaquette (c) 2022 Sofian Audry        :: info(@)sofianaudry(.)com
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

#include "Normalizer.h"
#include "EMA.h"

#include <algorithm>
#include <cmath>
#include <limits>

// ---- Constructors ----- //

Normalizer::Normalizer(float targetMean, float targetStd)
  : _infinite(true)
  , _tau_s(1.0)
  , _targetMean(targetMean)
  , _targetStd(std::fabs(targetStd))
  , _doClamp(false)
  , _clampNSig(0.0f)
  , _m1(0.0f)
  , _m2(0.0f)
  , _y(targetMean)
  , _n(0)
{
  init_states();
  clamp(kDefaultClampNSigmas);
}

Normalizer::Normalizer(double timeWindowSeconds,
                       float targetMean,
                       float targetStd)
  : _infinite(timeWindowSeconds <= 0.0)
  , _tau_s(_infinite ? 1.0 : timeWindowSeconds)
  , _targetMean(targetMean)
  , _targetStd(std::fabs(targetStd))
  , _doClamp(false)
  , _clampNSig(0.0f)
  , _m1(0.0f)
  , _m2(0.0f)
  , _y(targetMean)
  , _n(0)
{
  init_states();
  clamp(kDefaultClampNSigmas);
}

// ---- Time window / decay control ------ //

void Normalizer::timeWindow(double seconds)
{
  const bool was_infinite = _infinite;

  _infinite = (seconds <= 0.0);
  _tau_s    = _infinite ? 1.0 : seconds;

  // Switching from finite -> infinite:
  // forget previous EMA state and reseed from next sample.
  if (!was_infinite && _infinite) {
    _n = 0;
    init_states();
  }
}

double Normalizer::timeWindow() const { return _infinite ? 0.0 : _tau_s; }
bool   Normalizer::timeWindowIsInfinite() const { return _infinite; }

// ---- Reset ----- //

void Normalizer::reset()
{
  _n = 0;
  init_states();
}

// ---- Targets ------ //

void  Normalizer::targetMean(float m)   { _targetMean = m; }
float Normalizer::targetMean() const   { return _targetMean; }

void  Normalizer::targetStdDev(float s) { _targetStd = std::fabs(s); }
float Normalizer::targetStdDev() const { return _targetStd; }

// ---- Inspectors ----- //

float Normalizer::mean() const { return _m1; }

float Normalizer::variance() const
{
  const float v = _m2 - _m1 * _m1;
  return (v > 0.0f) ? v : 0.0f;
}

float Normalizer::stddev() const
{
  const float v = variance();
  return (v > 0.0f) ? std::sqrt(v) : 0.0f;
}

float Normalizer::value() const { return _y; }

bool Normalizer::isOutlier(float value, float nStdDev) const
{
  const float sd = stddev();
  if (!(sd > 0.0f))
    return false;

  const float z = (value - _m1) / sd;
  return std::fabs(z) >= std::fabs(nStdDev);
}

// ---- Main entry ------ //

float Normalizer::put(float x, double dt_seconds)
{
  update_stats(x, dt_seconds);
  return finalize(x);
}

// ---- Helpers ----------------------------------------------------------------

// Initialize the internal state
void Normalizer::init_states()
{
  _m1 = 0.0f;
  _m2 = 0.0f;
  _y  = _targetMean;
}

// Update running mean + second moment
void Normalizer::update_stats(float x, double dt_seconds)
{
  // First sample after reset / reseed.
  if (_n == 0) {
    if (_infinite) {
      // In infinite mode, seed directly from data.
      _m1 = x;
      _m2 = x * x;
    }
    // In finite mode, we keep prior (_m1/_m2 set in init_states()).
    _n = 1;
    return;
  }

  const float a  = ema_alpha(_infinite, _tau_s, _n, dt_seconds);
  const float xx = x * x;

  ema_apply_update(_m1, x,  a);
  ema_apply_update(_m2, xx, a);

  if (_n < std::numeric_limits<std::uint32_t>::max())
    ++_n;
}

// Compute normalized & mapped output
float Normalizer::finalize(float x)
{
  const float mu  = _m1;
  const float var = std::max(0.0f, _m2 - mu * mu);
  const float sd  = (var > 0.0f) ? std::sqrt(var) : 0.0f;

  const float z = (sd > 1e-12f) ? (x - mu) / sd : 0.0f;

  float y = _targetMean + z * _targetStd;

  if (_doClamp)
  {
    const float r  = _clampNSig * _targetStd;
    const float lo = _targetMean - r;
    const float hi = _targetMean + r;
    y = std::clamp(y, lo, hi);
  }

  _y = y;
  return _y;
}

// ---- Clamp ------ //

void Normalizer::clamp(float nStdDev)
{
  _doClamp   = true;
  _clampNSig = std::fabs(nStdDev);
}

void Normalizer::noClamp()         { _doClamp = false; }
bool Normalizer::isClamped() const { return _doClamp; }
