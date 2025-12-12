 /*
 * EMA.h
 *
 * Small helpers for exponential moving averages with a time window
 * and an optional infinite (cumulative) mode.
 *
 * (c) 2025 Luana Belinsky
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef EMA_H_
#define EMA_H_

#include <cstdint>
#include <cmath>

/**
 * Compute EMA alpha given:
 *  - infinite: whether window is infinite (cumulative)
 *  - tau_s:    time window in seconds (ignored when infinite == true)
 *  - n:        number of samples seen so far
 *  - dt_seconds: elapsed time since previous sample
 *
 * Behavior:
 *  - Infinite window: alpha = 1 / (n + 1)
 *  - Finite window:
 *      warmup:  alpha = 1 / (n + 1) until n ~= tau_s * sampleRate
 *      steady:  alpha = 2 / (N + 1) where N = tau_s * sampleRate
 */
inline float ema_alpha(bool infinite,
                       double tau_s,
                       std::uint32_t n,
                       double dt_seconds)
{
  const float smoothTime = infinite ? -1.0f : static_cast<float>(tau_s);

  // Infinite window: cumulative average alpha = 1/(n+1)
  if (smoothTime < 0.0f) {
    return 1.0f / (static_cast<float>(n) + 1.0f);
  }

  // Finite window.
  if (dt_seconds <= 0.0) {
    // No timing info: fallback to immediate update.
    return 1.0f;
  }

  const float sampleRate     = static_cast<float>(1.0 / dt_seconds);
  const float nSamplesTarget = smoothTime * sampleRate;

  if (static_cast<float>(n) < nSamplesTarget - 1.0f) {
    // Warm-up phase: simple 1/(n+1) average.
    return 1.0f / (static_cast<float>(n) + 1.0f);
  } else {
    // Steady-state EMA: ~2/(N+1), with N = smoothTime * sampleRate.
    return (nSamplesTarget > 1.0f
              ? 2.0f / (nSamplesTarget + 1.0f)
              : 1.0f);
  }
}

/**
 * Apply EMA update:
 *
 *   runningValue <- runningValue - alpha * (runningValue - newValue)
 */
inline void ema_apply_update(float& runningValue,
                             float  newValue,
                             float  alpha)
{
  runningValue -= alpha * (runningValue - newValue);
}

#endif // EMA_H_