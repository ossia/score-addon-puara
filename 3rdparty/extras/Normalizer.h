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

#ifndef NORMALIZER_H_
#define NORMALIZER_H_

#include <cstdint>

/*
 * Adaptive normalizer: normalizes values on-the-run using running
 * estimates of mean and standard deviation.
 *
 * The class supports:
 *  - Infinite mode (cumulative statistics)
 *  - Finite time window (EMA-based statistics)
 *
 * Workflow:
 *   1) Estimate mean and variance from incoming stream
 *   2) Compute z-score
 *   3) Remap to target mean and std deviation
 *   4) Optional clamp around target mean
 */

class Normalizer {
public:

  // Default target distribution (standard normal)
  static constexpr float kDefaultTargetMean   = 0.0f;
  static constexpr float kDefaultTargetStdDev = 1.0f;

  // Default clamp range (±3σ)
  static constexpr float kDefaultClampNSigmas = 3.0f;

  Normalizer(float targetMean = kDefaultTargetMean,
             float targetStd  = kDefaultTargetStdDev);

  Normalizer(double timeWindowSeconds,
             float targetMean = kDefaultTargetMean,
             float targetStd  = kDefaultTargetStdDev);

  virtual ~Normalizer() {}

  // ---- Time window control ---- //

  // seconds <= 0.0 -> infinite mode
  virtual void   timeWindow(double seconds);
  virtual double timeWindow() const;
  virtual bool   timeWindowIsInfinite() const;

  // ---- Reset ---- //

  // Reset internal statistics; next sample will reseed state.
  virtual void reset();

  // ---- Target distribution ---- //

  void  targetMean(float m);
  float targetMean() const;

  void  targetStdDev(float s);
  float targetStdDev() const;

  // ---- Inspectors ---- //

  float mean() const;
  float variance() const;
  float stddev() const;
  float value() const;

  float lowOutlierThreshold(float nStdDev = 1.5f) const;
  float highOutlierThreshold(float nStdDev = 1.5f) const;

  bool isOutlier(float value, float nStdDev = 1.5f) const;

  // ---- Clamp ---- //

  void clamp(float nStdDev = kDefaultClampNSigmas);
  void noClamp();
  bool isClamped() const;

  // ---- Main interface ---- //

  // dt_seconds is the elapsed time since the previous sample (seconds).
  virtual float put(float value, double dt_seconds);

protected:

  void  init_states();
  void  update_stats(float x, double dt_seconds);
  float finalize(float x);

  // Window config
  bool   _infinite;
  double _tau_s;

  // Target distribution
  float _targetMean;
  float _targetStd;

  // Clamp
  bool  _doClamp;
  float _clampNSig;

  // Internal statistics
  float         _m1;
  float         _m2;
  float         _y;
  std::uint32_t _n;
};

#endif // NORMALIZER_H_
