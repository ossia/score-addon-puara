#pragma once

#include <halp/audio.hpp>
#include <halp/controls.hpp>
#include <halp/meta.hpp>
#include <halp/sample_accurate_controls.hpp>
#include <puara/utils/calibration.h>
#include <puara/structs.h>

namespace puara_gestures::objects
{

class Calibration
{
public:
  halp_meta(name, "Magnetometer Calibration")
  halp_meta(category, "Gestures")
  halp_meta(c_name, "puara_calibration")
  halp_meta(description, "Calibrates a raw magnetometer stream to correct for hard and soft-iron distortions.")
  halp_meta(manual_url, "https://github.com/Puara/puara-gestures/")
  halp_meta(uuid, "0b46699b-5e17-4742-a585-e7d7ad9b22ca")

  struct ins
  {
    halp::val_port<"Raw Magnetometer", puara_gestures::Coord3D> raw_magnetometer;
    halp::toggle<"Record Data"> record_data;
    halp::accurate<halp::impulse_button<"Generate Calibration">> generate_calibration;
    halp::knob_f32<"Gravitation Field", halp::range{0.0, 1000.0, 234.0f}> gravitation_field;
  } inputs;

  struct outs
  {
    halp::val_port<"Calibrated Magnetometer", puara_gestures::Coord3D> calibrated_magnetometer;
    halp::val_port<"Points Collected", int> points_collected;
    halp::val_port<"Hard-Iron Bias", puara_gestures::Coord3D> hard_iron_bias;
    halp::val_port<"Soft-Iron Row 1", puara_gestures::Coord3D> soft_iron_row1;
    halp::val_port<"Soft-Iron Row 2", puara_gestures::Coord3D> soft_iron_row2;
    halp::val_port<"Soft-Iron Row 3", puara_gestures::Coord3D> soft_iron_row3;
  } outputs;

  halp::setup setup;
  void prepare(halp::setup info) { setup = info; }

  using tick = halp::tick;
  void operator()(tick t);

private:
  puara_gestures::utils::Calibration m_calibration_logic;
  bool m_is_recording = false;
  bool m_last_record_state = false;
};

}
