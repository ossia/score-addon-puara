/*
 * QuantileScaler.h
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

#ifndef QUANTILE_SCALER_H_
#define QUANTILE_SCALER_H_

#include <cstdint>

/*
 * Regularizes a signal into [0, 1] using adaptive quantile tracking.
 *
 * The scaler tracks low and high quantiles of the input distribution
 * using Robbins–Monro stochastic updates, and maps the input into [0, 1]
 * based on those quantiles. This makes it robust to outliers compared
 * to a simple min–max scaler.
 *
 * The class supports:
 *  - Infinite mode (no decay of quantiles)
 *  - Finite time window (EMA-based decay)
 */

class QuantileScaler {
public:
  // Default span (corresponds to percentage coverage of values in [0, 1]).
  static constexpr float kDefaultSpan = 0.99f;

  QuantileScaler();
  QuantileScaler(double timeWindowSeconds, float span = kDefaultSpan);

  virtual ~QuantileScaler() {}

  // ---- Time window control ---- //

  /// Sets the adaptation time window (in seconds). <= 0.0 => infinite mode.
  virtual void   timeWindow(double seconds);

  /// Returns the current time window (in seconds). 0.0 => infinite mode.
  virtual double timeWindow() const;

  /// Returns true if time window is infinite.
  virtual bool   timeWindowIsInfinite() const;

  // ---- Reset ---- //

  /// Resets the filter state.
  virtual void reset();

  // ---- Span / quantiles ---- //

  /// Sets the span (in [0, 1]) of the central mass to be covered.
  /// For example, span = 0.99 covers approximately 99% of values.
  virtual void span(float span);

  /// Returns the current span.
  virtual float span() const;

  /// Sets the low quantile level (in [0, 0.5]).
  virtual void lowQuantileLevel(float level);

  /// Returns the current low quantile level.
  virtual float lowQuantileLevel() const { return _lowQuantileLevel; }

  /// Sets the high quantile level (in [0.5, 1]).
  virtual void highQuantileLevel(float level);

  /// Returns the current high quantile level.
  virtual float highQuantileLevel() const { return (1.0f - _lowQuantileLevel); }

  // ---- Inspectors ---- //

  /// Returns the current low quantile estimate.
  float lowQuantile()  const { return _lowQuantile; }

  /// Returns the current high quantile estimate.
  float highQuantile() const { return _highQuantile; }

  /// Returns the current stddev proxy used to scale the quantile step size.
  float stddev() const { return _stddev; }

  /// Returns the last scaled output in [0, 1].
  float value() const { return _value; }

  // ---- Main interface ---- //

  /**
   * Pushes a new value and returns the scaled output.
   *
   * @param x          input value
   * @param dt_seconds elapsed time since the previous sample (seconds).
   *                   If dt_seconds <= 0 in finite-window mode, the
   *                   update falls back to immediate adjustment.
   */
  virtual float put(float x, double dt_seconds);

protected:
  void  init_states();

  // Mapping between span (central mass) and low quantile level.
  static float lowQuantileLevelToSpan(float level);
  static float spanToLowQuantileLevel(float span);

protected:
  // Time window (in seconds).
  bool   _infinite;
  double _tau_s;

  // Low quantile level (in [0, 0.5]).
  float _lowQuantileLevel;

  // Quantile estimators.
  float _lowQuantile;   // q_low
  float _highQuantile;  // q_high

  // Stddev estimator (based on deviation from mid-quantile).
  float _stddev;

  // Last scaled value in [0, 1].
  float _value;

  // Number of samples since last reset.
  std::uint32_t _n;
};

#endif // QUANTILE_SCALER_H_
