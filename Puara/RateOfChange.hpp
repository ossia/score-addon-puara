#pragma once
#include "halp_utils.hpp"

#include <halp/controls.hpp>
#include <halp/meta.hpp>

#include <vector>
#include <cstddef>
#include <cmath>

namespace puara_gestures::objects
{
// rate of change
class RateOfChange
{
public:
  halp_meta(name, "Rate of change")
  halp_meta(category, "Analysis/Data")
  halp_meta(c_name, "Rate of change")
  halp_meta(author, "Luana Belinsky")
  halp_meta(
      description,
      "Calculates the rate of change (delta) of an input signal. "
      "Supports a fixed sample window or a time window, and output in different time units.")
  halp_meta(uuid, "7137a3bc-b3c0-43bc-9774-61c0e272db71")

  //==== output units (used by combobox) ===//
  enum
  {
    PerMillisecond,
    PerSecond,
    PerMinute,
    PerHour
  };

  struct
  {
    // Signal input
    halp::data_port<"Signal", "Int or float. Input signal to analyze", float> signal;

    // Sample window (N) 
    halp::spinbox_i32<"Sample window (N)", halp::range{2, 1000000, 1}> sample_window{2};

    // Output units
    struct
    {
      halp__enum_combobox(
          "Output units",
          PerSecond,        // default
          PerMillisecond,
          PerSecond,
          PerMinute,
          PerHour)
    } output_units;
  } inputs;

  struct
  {
    halp::data_port<"Rate of change (delta/time)",
                    "Float. Units as per 'Output units'.",
                    float>
        out;
  } outputs;

  halp::setup setup;
  void prepare(halp::setup info);

  using tick = halp::tick;
  void operator()(halp::tick t);

  // derivative across the current window: (x_last - x_first) / sum(dt)
  float delta_per(int unit) const;

private:
  // each sample pair contains value and dt
  struct Sample
  {
    float  x;   // value
    double dt;  // delta time since previous sample (seconds)
  };

  // ring buffer storage
  std::vector<Sample> _buf;
  const std::size_t _physCap = 1000000; // physical capacity (allocated size)
  std::size_t _cap     = 0; // logical max samples 
  std::size_t _head    = 0; // index of oldest in ring
  std::size_t _size    = 0; // number of valid samples in window (<= _physCap)
  double      _sumDt   = 0.0; // sum of dt for all samples except the oldest

  // ---------- helpers ---------- //
  // return a const reference to the nth newest sample (0 = newest)
  const Sample& nth_newest_sample(std::size_t n) const;

  // drop the oldest sample from the window
  void drop_oldest_sample();

  // push one new sample (x, dt) into the ring buffer
  void push(float x, double dt);

  // enforce sample window
  void apply_window_policy();

  // react to changes in parameters
  void update_window_from_params();

  // parameter watchers
  halp::ParameterWatcher<int> sample_window_size;
};

} // namespace puara_gestures::objects
