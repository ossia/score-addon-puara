/*
 * MinMaxScaler.h
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

#ifndef MIN_MAX_SCALER_H_
#define MIN_MAX_SCALER_H_

#include <cstdint>

/*
 * Adaptive min-max scaler: rescales a stream of values into [0, 1]
 * using running estimates of minimum and maximum.
 *
 * The class supports:
 *  - Infinite mode (hard running min/max, no decay)
 *  - Finite time window (EMA-based decay of min/max)
 *
 * Smoothed versions of min/max are tracked to avoid abrupt jumps
 * when extremes change.
 */

class MinMaxScaler {
public:
  // ---- Constructors ----- //

  /// Construct a scaler with infinite time window (no decay).
  MinMaxScaler();

  /**
   * Construct a scaler with a given time window (in seconds).
   * A time window <= 0.0 selects infinite mode.
   */
  MinMaxScaler(double timeWindowSeconds);

  virtual ~MinMaxScaler() {}

  // ---- Time window / decay control ------ //

  /// Sets the time window (seconds). <= 0.0 => infinite mode.
  virtual void timeWindow(double seconds);

  /// Returns the time window (seconds). 0.0 => infinite mode.
  virtual double timeWindow() const;

  /// Returns true if the time window is infinite (no decay).
  virtual bool timeWindowIsInfinite() const;

  // ---- Reset ----- //

  /// Resets internal statistics and state.
  virtual void reset();

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
  /// Initialize the internal state
  void init_states();

  // Window configuration
  bool   _infinite;
  double _tau_s;

  // Min/max estimates
  float _minValue;
  float _maxValue;

  // Smoothed min/max
  float _smoothedMinValue;
  float _smoothedMaxValue;

  // Last output value in [0, 1]
  float _value;

  // Sample counter
  std::uint32_t _n;
};

#endif // MIN_MAX_SCALER_H_