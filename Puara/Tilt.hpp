#pragma once

#include <halp/audio.hpp>
#include <halp/controls.hpp>
#include <halp/meta.hpp>
#include <puara/descriptors/tilt.h>

namespace puara_gestures::objects
{
class Tilt
{
public:
  halp_meta(name, "Tilt")
  halp_meta(category, "Gestures")
  halp_meta(c_name, "puara_tilt")
  halp_meta(uuid, "13fd4da6-3ebb-48dd-ac81-3de0aced5d32")

  struct ins
  {
    halp::val_port<"Acceleration", puara_gestures::Coord3D> accel;
    halp::val_port<"Gyrosocope", puara_gestures::Coord3D> gyro;
    halp::val_port<"Magnetometer", puara_gestures::Coord3D> mag;
  } inputs;

  struct
  {
    halp::val_port<"Output", float> output;
  } outputs;

  halp::setup setup;
  void prepare(halp::setup info) { setup = info; }

  using tick = halp::tick;
  void operator()(halp::tick t);

  puara_gestures::Tilt impl;
};

}
