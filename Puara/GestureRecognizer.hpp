#pragma once
#include <halp/audio.hpp>
#include <halp/controls.hpp>
#include <halp/meta.hpp>
#include <puara/gestures.h>

namespace puara_gestures::objects
{
class GestureRecognizer
{
public:
  halp_meta(name, "Gesture recognizer")
  halp_meta(category, "Gestures")
  halp_meta(c_name, "puara_roll")
  halp_meta(uuid, "3135b5e0-39a6-4151-8ae4-a657d577eab8")

  struct ins
  {
    halp::val_port<"Acceleration", puara_gestures::Coord3D> accel;
    halp::val_port<"Gyrosocope", puara_gestures::Coord3D> gyro;
    halp::val_port<"Magnetometer", puara_gestures::Coord3D> mag;
  } inputs;

  struct
  {
    halp::val_port<"Jab", puara_gestures::Coord3D> jab;
    halp::val_port<"Shake", puara_gestures::Coord3D> shake;
    halp::val_port<"Tilt", float> tilt;
    halp::val_port<"Roll", float> roll;
  } outputs;

  halp::setup setup;
  void prepare(halp::setup info) { setup = info; }

  using tick = halp::tick;
  void operator()(halp::tick t);

  puara_gestures::Jab3D jab;
  puara_gestures::Shake3D shake;
  puara_gestures::Tilt tilt;
  puara_gestures::Roll roll;
};

}
