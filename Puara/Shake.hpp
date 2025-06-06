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

    halp::knob_f32<"Integrator Frequency (Hz)",halp::range{0.0,200.0,10.0f}>integrator_frequency;

    //new params
    halp::knob_f32<"Fast Leak",halp::range{0.0,1.0,0.6}>fast_leak_param;
    halp::knob_f32<"Slow Leak",halp::range{0.0,1.0,0.3}>slow_leak_param;
    //for threshold
    halp::knob_f32<"Activation Threshold",halp::range{0.0,1.0,0.1}>activation_threshold_param;


  } inputs;

  struct outputs
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
