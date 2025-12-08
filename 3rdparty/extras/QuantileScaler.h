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
 * Adaptive quantile scaler: rescales a stream of values into [0, 1]
 * using running estimates of low and high quantiles.
 *
 * The class supports:
 *  - Infinite mode (no decay)
 *  - Finite time window (EMA-based decay)
 *  - Symmetric quantile tracking (span from center)
 *
 * Implementation based on Robbins–Monro stochastic approximation.
 */

class QuantileScaler {
public:
  // Default span covering 99% of distribution (0.5% tails on each side)
  static constexpr float kDefaultSpan = 0.99f;

  // ---- Constructors ----- //

  /// Construct a scaler with infinite time window and default span.
  QuantileScaler();

  /**
   * Construct a scaler with a given time window (seconds) and span.
   * A time window <= 0.0 selects infinite mode.
   * Span ∈ [0, 1] controls coverage (1.0 = full min-max).
   */
  QuantileScaler(double timeWindowSeconds, float span = kDefaultSpan);

  virtual ~QuantileScaler() {}

  // ---- Time window / decay control ------ //

  /// Sets the time window (seconds). <= 0.0 => infinite mode.
  virtual void timeWindow(double seconds);

  /// Returns the time window (seconds). 0.0 => infinite mode.
  virtual double timeWindow() const;

  /// Returns true if the time window is infinite (no decay).
  virtual bool timeWindowIsInfinite() const;

  // ---- Reset ----- //

  /// Resets to initial uninitialized state (waits for first sample).
  virtual void reset();

  // ---- Span / quantiles API ------ //

  /// Sets the span (coverage) ∈ [0, 1]. 1.0 = full min-max.
  virtual void span(float span);

  /// Returns the current span.
  virtual float span() const;

  /// Sets low quantile level directly (e.g., 0.01 for 1st percentile).
  virtual void lowQuantileLevel(float level);

  /// Sets high quantile level directly (e.g., 0.99 for 99th percentile).
  virtual void highQuantileLevel(float level);

  // ---- Inspectors ----- //

  /// Returns the last scaled output in [0, 1].
  float value() const { return _value; }

  // ---- Main entry ------ //

  /**
   * Pushes a new value and returns the scaled output in [0, 1].
   *
   * @param x          input value
   * @param dt_seconds elapsed time since the previous sample (seconds).
   *                   If dt_seconds <= 0 in finite-window mode, the
   *                   update falls back to immediate adjustment.
   */
  virtual float put(float x, double dt_seconds);

private:
  // ---- Helpers ----- //

  /// Initialize the internal state (uninitialized).
  void init_states();

  /// Static helpers for span ↔ quantile level conversion.
  static float lowQuantileLevelToSpan(float level);
  static float spanToLowQuantileLevel(float span);

  // Window configuration
  bool   _infinite;
  double _tau_s;

  // Quantile configuration
  float _lowQuantileLevel;  // τ (e.g., 0.01 for 1st percentile)

  // State variables
  float _lowQuantile;
  float _highQuantile;
  float _stddev;            // Running estimate of deviation
  float _value;             // Last output ∈ [0, 1]

  // Sample counter
  std::uint32_t _n;
};

#endif // QUANTILE_SCALER_H_