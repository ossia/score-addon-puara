#pragma once

#include "puara-roll.h"

#include <halp/audio.hpp>
#include <halp/controls.hpp>
#include <halp/meta.hpp>

namespace Example
{
struct CoordInput
{
  puara_gestures::Coord3D value;
};

class Puara
{
public:
  halp_meta(name, "Roll")
  halp_meta(category, "Gesture")
  halp_meta(c_name, "puara_roll")
  halp_meta(uuid, "f8a4afbc-4b86-41b2-bbce-d869eb78b2b6")

  struct ins
  {
    CoordInput accel;
    CoordInput gyro;
    CoordInput mag;
  } inputs;

  struct
  {
    halp::val_port<"Output", float> output;
  } outputs;

  using setup = halp::setup;
  void prepare(halp::setup info)
  {
    // Initialization, this method will be called with buffer size, etc.
  }

  // Do our processing for N samples
  using tick = halp::tick;

  // Defined in the .cpp
  void operator()(halp::tick t);

  puara_gestures::Roll r;
};

}
