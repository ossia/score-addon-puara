#pragma once

#include <halp/controls.hpp>
#include <halp/meta.hpp>
#include <cmath>

namespace puara_gestures::objects
{

class WalkerAvnd
{
public:
  halp_meta(name, "Walker")
  halp_meta(category, "Control/Generators")
  halp_meta(c_name, "puara_walker_avnd")
  halp_meta(description, "Simulates a walker on a globe based on angle and velocity.")
  halp_meta(uuid, "d307f85a-5d70-4882-901f-f748ed4fe8f6")

  struct
  {
    halp::val_port<"Angle (degrees)", double> angle;
    halp::val_port<"Velocity", double> velocity;
    halp::val_port<"On Water", bool> on_water;
    halp::knob_f32<"Initial Latitude", halp::range{-90.0, 90.0, 0.0}> initial_latitude;
    halp::knob_f32<"Initial Longitude", halp::range{-180.0, 180.0, 0.0}> initial_longitude;
    halp::knob_f32<"Water Speed Factor", halp::range{0.1, 10.0, 5.0}> water_speed_factor;
    halp::impulse_button<"Reset Position"> reset;
  } inputs;

  struct
  {
    halp::val_port<"Latitude", double> latitude;
    halp::val_port<"Longitude", double> longitude;
  } outputs;

  void operator()();

  WalkerAvnd();

private:
  void reset_position();
  double m_current_latitude{0.0};
  double m_current_longitude{0.0};
};

}
