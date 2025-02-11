#pragma once
#include <halp/audio.hpp>
#include <halp/controls.hpp>
#include <halp/meta.hpp>
#include <puara/gestures.h>
#include "3rdparty/BioData/src/Heart.h"
#include "3rdparty/BioData/src/SkinConductance.h"

namespace puara_gestures::objects
{
class GestureRecognizer
{
public:
  halp_meta(name, "Gesture recognizer")
  halp_meta(category, "Gestures")
  halp_meta(c_name, "puara_gestures")
  halp_meta(
      author,
      "Puara authors, Edu Meneses, Rochana Fardon, Sarah Al Mamoun, Joseph Malloch, "
      "Maggie Needham")
  halp_meta(manual_url, "https://ossia.io/score-docs/processes/gestures.html")
  halp_meta(uuid, "3135b5e0-39a6-4151-8ae4-a657d577eab8")

  struct ins
  {
    halp::val_port<"Acceleration", puara_gestures::Coord3D> accel;
    halp::val_port<"Gyrosocope", puara_gestures::Coord3D> gyro;
    halp::val_port<"Magnetometer", puara_gestures::Coord3D> mag;
    halp::val_port<"Heart_signal", float> heart_signal;
    halp::val_port<"GSR_signal", float> GSR_signal;

    halp::knob_f32<"GSR threshold", halp::range{0., 1., 0.}> GSR_threshold;
  } inputs;

  struct
  {
    halp::val_port<"Jab", puara_gestures::Coord3D> jab;
    halp::val_port<"Shake", puara_gestures::Coord3D> shake;
    halp::val_port<"Tilt", float> tilt;
    halp::val_port<"Roll", float> roll;
    halp::val_port<"Heart_raw", float> heart_raw;
    halp::val_port<"Heart_normalized", float> heart_normalized;
    halp::val_port<"Heart_beatDetected", bool> heart_beat;
    halp::val_port<"Heart_bpm", float> heart_bpm;
    halp::val_port<"Heart_bpmChange", float> heart_bpmChange;
    halp::val_port<"Heart_amplitudeChange", float> heart_amplitudeChange;
    halp::val_port<"GSR_raw", float> GSR_raw;
    halp::val_port<"GSR_SCR", float> GSR_SCR;
    halp::val_port<"GSR_SCL", float> GSR_SCL;
  } outputs;

  halp::setup setup;
  void prepare(halp::setup info) { setup = info; }

  using tick = halp::tick;
  void operator()(halp::tick t);

  puara_gestures::Jab3D jab;
  puara_gestures::Shake3D shake;
  puara_gestures::Tilt tilt;
  puara_gestures::Roll roll;
  Heart heart;
  SkinConductance gsr;
};

}
