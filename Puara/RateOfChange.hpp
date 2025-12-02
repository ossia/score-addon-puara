#pragma once
#include "halp_utils.hpp"

#include <halp/controls.hpp>
#include <halp/meta.hpp>

#include <cmath>

#include <cstddef>
#include <vector>

namespace puara_gestures::objects
{
// rate of change with sample count OR time window
class RateOfChange
{
public:
  //==== window mode ===//
  enum class WindowMode
  {
    SampleCount, ///< Use fixed number of samples
    TimeWindow   ///< Use fixed time window
  };

  //==== output units (used by combobox) ===//
  enum
  {
    PerMillisecond,
    PerSecond,
    PerMinute,
    PerHour
  };

  halp_meta(name, "Rate of change")
  halp_meta(category, "Analysis/Data")
  halp_meta(c_name, "rate_of_change")
  halp_meta(author, "Luana Belinsky")
  halp_meta(
      description,
      "Calculates the rate of change (delta) of an input signal. "
      "Supports either a fixed sample window OR a time window, and output in different "
      "time units. "
      "When using time window mode, if no new data arrives, the last value is held "
      "constant.")
  halp_meta(uuid, "7137a3bc-b3c0-43bc-9774-61c0e272db71")

  struct
  {
    // Signal input
    halp::data_port<"Signal", "Int or float. Input signal to analyze", float> signal;

    // Window mode selection using enum_t
    halp::enum_t<WindowMode, "Window mode"> window_mode{WindowMode::SampleCount};

    // Sample window (N) - visible when WindowMode::SampleCount is selected
    halp::spinbox_i32<"Sample count", halp::range{2, 1000000, 50}> sample_count;

    // Time window (seconds) - visible when WindowMode::TimeWindow is selected
    halp::spinbox_f32<"Time window (s)", halp::range{0.001, 3600.0, 1.0}> time_window;

    // Output units using combobox style
    struct
    {
      halp__enum_combobox(
          "Output units",
          PerSecond, // default
          PerMillisecond, PerSecond, PerMinute, PerHour)
    } output_units;

  } inputs;

  struct
  {
    halp::data_port<
        "Rate of change (delta/time)", "Float. Units as per 'Output units'.", float>
        out;

    // Diagnostic outputs
    halp::data_port<
        "Actual samples in window", "Int. Current number of samples in analysis window.",
        int>
        actual_samples;
    halp::data_port<
        "Actual window time span (s)",
        "Float. Actual time span covered by window in seconds.", float>
        actual_time;
  } outputs;

  halp::setup setup;
  void prepare(halp::setup info);

  using tick = halp::tick;
  void operator()(halp::tick t);

  // derivative across the current window: (x_last - x_first) / sum(dt)
  float delta_per(int unit) const;

private:
  // each sample pair contains value, dt, and absolute time
  struct Sample
  {
    float x;   // value
    double dt; // delta time since previous sample (seconds)
    double t;  // absolute time since start (seconds)
  };

  // ring buffer storage
  std::vector<Sample> _buf;
  const std::size_t _physCap = 1000000; // physical capacity (allocated size)
  std::size_t _cap = 0;                 // logical max samples (for sample count mode)
  std::size_t _head = 0;                // index of oldest in ring
  std::size_t _size = 0;       // number of valid samples in window (<= _physCap)
  double _sumDt = 0.0;         // sum of dt for all samples except the oldest
  double _sumTime = 0.0;       // total time span of window (sum of all dt)
  double _maxTime = 1.0;       // maximum time window (seconds)
  double _totalTime = 0.0;     // absolute time since start
  float _lastValue = 0.0f;     // Last received value (for holding)
  double _lastValueTime = 0.0; // When we last received a value

  // ---------- helpers ---------- //
  // return a const reference to the nth newest sample (0 = newest)
  const Sample& nth_newest_sample(std::size_t n) const;

  // drop the oldest sample from the window
  void drop_oldest_sample();

  // push one new sample (x, dt) into the ring buffer
  void push(float x, double dt);

  // enforce window policy based on current mode
  void apply_window_policy();

  // react to changes in parameters
  void update_window_from_params();

  // parameter watchers
  halp::ParameterWatcher<int> sample_count_watcher;
  halp::ParameterWatcher<int> time_window_watcher;
  halp::ParameterWatcher<WindowMode> window_mode_watcher;
  halp::ParameterWatcher<int> output_units_watcher;
};

} // namespace puara_gestures::objects