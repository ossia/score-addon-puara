#pragma once

/*
 * Normalizer.h
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


#include <cstdint>

class Normalizer {
public:
  // defaults
  static constexpr float  kDefaultTargetMean     = 0.0f;
  static constexpr float  kDefaultTargetStdDev   = 1.0f;
  static constexpr float  kDefaultClampNSigmas   = 3.0f;

  // constructors
  explicit Normalizer(float targetMean = kDefaultTargetMean,
                      float targetStd  = kDefaultTargetStdDev);
  Normalizer(double timeWindowSeconds,
             float  targetMean = kDefaultTargetMean,
             float  targetStd  = kDefaultTargetStdDev);

  // time window / decay
  void   timeWindow(double seconds);   // <=0 --> infinite
  double timeWindow() const;           // 0 if infinite
  bool   timeWindowIsInfinite() const;

  // reset
  void   reset();

  // main entry: feed one sample with known dt (seconds)
  float  put(float x, double dt_seconds);

  // targets
  void   targetMean(float m);
  float  targetMean() const;

  void   targetStdDev(float s);
  float  targetStdDev() const;

  // stats
  float  mean() const;
  float  variance() const;
  float  stddev() const;

  // last output
  float  value() const;

  // clamp
  void   clamp(float nStdDev = kDefaultClampNSigmas);
  void   noClamp();
  bool   isClamped() const;

  // outliers relative to target distribution
  float  lowOutlierThreshold (float nStdDev) const;
  float  highOutlierThreshold(float nStdDev) const;
  bool   isOutlier(float value, float nStdDev) const;

private:
  // helpers
  static float alpha_from_tau_dt(double tau_s, double dt);
  void   init_states();                // initialize internal stats/output
  void   update_stats(float x, float a);
  float  finalize(float x);            // z-score --> retarget --> clamp

private:
  // config / targets
  float   _targetMean = kDefaultTargetMean;
  float   _targetStd  = kDefaultTargetStdDev;

  // decay control
  bool    _infinite = true;            // true --> cumulative (no decay)
  double  _tau_s    = 1.0;             // EMA time constant (s) when finite

  // cumulative-mode helpers
  uint64_t _n      = 0;                // sample count in infinite mode
  bool     _reseed = true;             // reseed from first sample after toggle

  // running stats (moments)
  float   _m1 = 0.0f;                  // E[x]
  float   _m2 = 0.0f;                  // E[x^2]

  // clamp
  bool    _doClamp   = true;
  float   _clampNSig = kDefaultClampNSigmas;

  // last output
  float   _y = 0.0f;
};
