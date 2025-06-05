#pragma once

#include <halp/audio.hpp>
#include <halp/controls.hpp>
#include <halp/meta.hpp>
#include <puara/descriptors/shake.h>

namespace puara_gestures::objects
{
class Shake
{
public:
  halp_meta(name, "Shake")
  halp_meta(category, "Gestures")
  halp_meta(c_name, "puara_shake")
  halp_meta(uuid, "ecb5dc46-164c-46a0-a01e-90e3c3bd7527")

  struct ins
  {
    halp::val_port<"Acceleration", puara_gestures::Coord3D> accel;



    halp::val_port<"Integrator Frequency",float>integrator_frequency{10.0f};
  } inputs;

  struct
  {
    halp::val_port<"Output", float> output;
  } outputs;

  halp::setup setup;
  void prepare(halp::setup info) { setup = info; }

  using tick = halp::tick;
  void operator()(halp::tick t);

  puara_gestures::Shake3D impl;
};

}
