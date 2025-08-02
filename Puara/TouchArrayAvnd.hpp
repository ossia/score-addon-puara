#pragma once

#include <halp/audio.hpp>
#include <halp/controls.hpp>
#include <halp/mappers.hpp>
#include <halp/meta.hpp>
#include <puara/descriptors/touchArrayGestureDetector.h>

#include <ratio>
#include <string>
#include <vector>

constexpr int TAGD_MAX_BLOBS = 3;
constexpr int TAGD_TOUCH_SIZE_EDGE = 2;

namespace puara_gestures::objects
{

class TouchArrayAvnd
{
public:
  halp_meta(name, "Touch Array Gestures")
  halp_meta(category, "Gestures")
  halp_meta(c_name, "puara_touch_array_avnd")
  halp_meta(
      description,
      "Detects touch averages, brush, and rub gestures from a linear touch array input.")
  halp_meta(
      manual_url,
      "https://github.com/Puara/puara-gestures/blob/main/include/puara/descriptors/"
      "touchArrayGestureDetector.h")
  halp_meta(uuid, "9d09a014-1f61-4051-97e6-aa9971a616e9")

  struct ins
  {
    halp::val_port<"Touch Array", std::vector<int>> touch_array_input;
    halp::knob_f32<"Brush/Rub Integrator Leak", halp::range{0.0, 1.0, 0.7}>
        common_integrator_leak;
    struct CommonIntegratorFreqParam
        : halp::knob_f32<
              "Brush/Rub Integrator Freq (Hz)", halp::range{1.0, 200.0, 100.0f}>
    {
      using mapper = halp::log_mapper<std::ratio<85, 100>>;
    } common_integrator_frequency;

  } inputs;

  struct outputs
  {
    halp::val_port<"Total Touch Avg", float> total_touch_avg{0.0f};
    halp::val_port<"Top Touch Avg", float> top_touch_avg{0.0f};
    halp::val_port<"Middle Touch Avg", float> middle_touch_avg{0.0f};
    halp::val_port<"Bottom Touch Avg", float> bottom_touch_avg{0.0f};

    halp::val_port<"Total Brush", float> total_brush{0.0f};
    halp::val_port<"Total Rub", float> total_rub{0.0f};

  } outputs;

  void operator()();

  puara_gestures::TouchArrayGestureDetector<TAGD_MAX_BLOBS, TAGD_TOUCH_SIZE_EDGE> impl;

  float m_prev_common_integrator_leak{-1.f};
  int m_prev_common_integrator_frequency{-1};
};

}
