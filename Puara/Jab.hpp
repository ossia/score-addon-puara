#pragma once

#include <halp/audio.hpp>
#include <halp/controls.hpp>
#include <halp/meta.hpp>
#include <puara/descriptors/jab.h>

namespace puara_gestures::objects
{
class Jab
{
public:
  halp_meta(name, "Jab")
  halp_meta(category, "Gestures")
  halp_meta(c_name, "puara_jab")
  halp_meta(uuid, "70cf4250-d66e-4a78-824f-4b51fb3d0075")

  struct ins
  {
    halp::val_port<"Acceleration", puara_gestures::Coord3D> accel;
    // halp::val_port<"Gyrosocope", puara_gestures::Coord3D> gyro;
    // halp::val_port<"Magnetometer", puara_gestures::Coord3D> mag;
  } inputs;

  struct
  {
    halp::val_port<"Output", puara_gestures::Coord3D> output;
  } outputs;

  halp::setup setup;
  void prepare(halp::setup info) { setup = info; }

  using tick = halp::tick;
  void operator()(halp::tick t);

  puara_gestures::Jab3D impl;
};

}
