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

#include <algorithm>
#include <cmath>

// ---- Constructors -----//

Normalizer::Normalizer(float targetMean, float targetStd)
  : _infinite(true)
  , _tau_s(1.0)
  , _targetMean(targetMean)
  , _targetStd (std::fabs(targetStd))
  , _reseed(true)  // start infinite mode seeded by first sample
{
  init_states();
  clamp(kDefaultClampNSigmas);
}

Normalizer::Normalizer(double timeWindowSeconds, float targetMean, float targetStd)
  : _infinite(timeWindowSeconds <= 0.0)
  , _tau_s(_infinite ? 1.0 : timeWindowSeconds)
  , _targetMean(targetMean)
  , _targetStd (std::fabs(targetStd))
  , _reseed(_infinite)  // reseed from first sample if infinite
{
  init_states();
  clamp(kDefaultClampNSigmas);
}

// ---- Time window / decay control ------//

void Normalizer::timeWindow(double seconds)
{
  const bool was_infinite = _infinite;
  _infinite = (seconds <= 0.0);
  _tau_s    = _infinite ? 1.0 : seconds;

  // EMA → infinite: forget prior mass and reseed from the next sample
  if (!was_infinite && _infinite) {
    _n      = 0;
    _reseed = true;
  }
}

double Normalizer::timeWindow() const { return _infinite ? 0.0 : _tau_s; }
bool   Normalizer::timeWindowIsInfinite() const { return _infinite; }

// ---- Reset -----//

void Normalizer::reset()
{
  // reset moments; for infinite, reseed from first sample
  _n      = 0;
  _reseed = _infinite;
  init_states();
}

// ---- Targets ------//

void  Normalizer::targetMean(float m)   { _targetMean = m; }
float Normalizer::targetMean()    const { return _targetMean; }

void  Normalizer::targetStdDev(float s) { _targetStd = std::fabs(s); }
float Normalizer::targetStdDev()  const { return _targetStd; }

// ---- Inspectors -----//

float Normalizer::mean()     const { return _m1; }
float Normalizer::variance() const
{
  const float v = _m2 - _m1 * _m1;
  return (v > 0.0f) ? v : 0.0f;
}
float Normalizer::stddev()   const { return std::sqrt(variance()); }
float Normalizer::value()    const { return _y; }

// thresholds around target distribution
float Normalizer::lowOutlierThreshold (float nStdDev) const
{ return _targetMean - std::fabs(nStdDev) * _targetStd; }

float Normalizer::highOutlierThreshold(float nStdDev) const
{ return _targetMean + std::fabs(nStdDev) * _targetStd; }

bool Normalizer::isOutlier(float value, float nStdDev) const
{
  return (value < lowOutlierThreshold(nStdDev)) ||
         (value > highOutlierThreshold(nStdDev));
}

// ---- Main entry ------//

float Normalizer::put(float x, double dt_seconds)
{
  // EMA coefficient from (tau, dt). Ignored in infinite mode.
  const float a = _infinite ? 0.0f : alpha_from_tau_dt(_tau_s, dt_seconds);

  update_stats(x, a);   // update E[x], E[x^2]
  return finalize(x);   // z-score --> retarget --> clamp
}

// ---- Helpers ----------------------------------------------------------------

// Convert (tau, dt) to EMA alpha: y <-- y + a*(x - y)
// a = 1 - exp(-dt/tau).  tau --> infinite or dt --> 0 --> a≈0 (slow); small tau → larger a (fast).
float Normalizer::alpha_from_tau_dt(double tau_s, double dt)
{
  if (tau_s <= 0.0 || dt <= 0.0) return 1.0f; // immediate (no smoothing)
  const double x = -dt / tau_s;
  return static_cast<float>(1.0 - std::exp(x));
}

// Initialize the internal state
void Normalizer::init_states()
{
  if (_infinite) {
    // cumulative mode --> start from data (first sample reseeds)
    _m1 = 0.0f;
    _m2 = 0.0f;
  } else {
    // EMA mode --> start from target prior
    _m1 = _targetMean;
    _m2 = _targetMean * _targetMean + _targetStd * _targetStd; // mean^2 + stddev^2
  }
  _y = _targetMean; // keep output centered initially
}

// Update running mean + second moment
void Normalizer::update_stats(float x, float a)
{
  if (_infinite) {
    // cumulative (no decay)
    if (_reseed || _n == 0) {
      _m1 = x;
      _m2 = x * x;
      _n  = 1;
      _reseed = false;
      return;
    }
    ++_n;
    const float invN = 1.0f / static_cast<float>(_n);
    _m1 += (x     - _m1) * invN;   // E[x]
    _m2 += (x*x   - _m2) * invN;   // E[x^2]
  } else {
    // EMA (finite window)
    a = std::clamp(a, 0.0f, 1.0f);
    _m1 = (1.0f - a) * _m1 + a * x;        // E[x]
    _m2 = (1.0f - a) * _m2 + a * (x * x);  // E[x^2]
  }
}

// Compute normalized & mapped output
// 1) z-score with current mean/std, 2) map to target mean/std, 3) optional clamp
float Normalizer::finalize(float x)
{
  const float mu  = _m1;
  const float var = std::max(0.0f, _m2 - mu * mu);
  const float sd  = std::sqrt(var);

  // If sd is ~0 (flat input), treat as zero-variance to avoid blow-ups.
  const float z = (sd > 1e-12f) ? (x - mu) / sd : 0.0f;

  // Retarget to desired output distribution
  float y = _targetMean + z * _targetStd;

  // Optional clamp around target mean
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

// ---- Clamp ------//

void Normalizer::clamp(float nStdDev)
{
  _doClamp   = true;
  _clampNSig = std::fabs(nStdDev);
}

void Normalizer::noClamp()         { _doClamp = false; }
bool Normalizer::isClamped() const { return _doClamp; }
