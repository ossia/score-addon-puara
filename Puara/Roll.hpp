#pragma once

#include <halp/audio.hpp>
#include <halp/controls.hpp>
#include <halp/meta.hpp>
#include <puara/descriptors/roll.h>
#include <cmath>
//changes --yash

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


namespace puara_gestures::objects
{
class Roll
{
public:
  halp_meta(name, "Roll")
  halp_meta(category, "Gestures")
  halp_meta(c_name, "puara_roll")
  halp_meta(uuid, "c8060aec-462b-4e3c-bb43-fa8fd1d4cdec")

  struct ins
  {
    halp::val_port<"Acceleration", puara_gestures::Coord3D> accel;
    halp::val_port<"Gyroscope", puara_gestures::Coord3D> gyro;
    halp::val_port<"Magnetometer", puara_gestures::Coord3D> mag;


    //changes --  defneing boolean toggles for post processing

    halp::toggle<"Enable Unwrap", halp::toggle_setup{.init = true}> enable_unwrap;
    halp::toggle<"Enable Smooth", halp::toggle_setup{.init = true}> enable_smooth;
    halp::toggle<"Enable Wrap", halp::toggle_setup{.init = false}> enable_wrap;

  } inputs;

  struct
  {
    // output:: single float value representing roll angle in radians
    halp::val_port<"Output", float> output;
  } outputs;

  halp::setup setup;
  void prepare(halp::setup info) { setup = info; }

  using tick = halp::tick;
  void operator()(halp::tick t);

  puara_gestures::Roll impl;
};
}

