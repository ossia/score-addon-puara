#pragma once

#include <halp/controls.hpp>
#include <halp/meta.hpp>

#include <vector>

namespace puara_gestures::objects
{

class CompassAvnd
{
public:
  halp_meta(name, "Compass")
  halp_meta(category, "Analysis/Puara")
  halp_meta(c_name, "puara_compass_avnd")
  halp_meta(description, "Calculates angles from two N-dimensional polar arrays.")
  halp_meta(manual_url, "https://github.com/dav0dea/goofi-pipe")
  halp_meta(uuid, "b0a377c7-0c7c-4525-be9a-2c3d0e67d41c")

  struct ins
  {
    halp::val_port<"Pole 1", std::vector<double>> pole1;
    halp::val_port<"Pole 2", std::vector<double>> pole2;
  } inputs;

  struct outs
  {
    halp::val_port<"Angles (degrees)", std::vector<double>> angles;
  } outputs;

  void operator()();
};

}
