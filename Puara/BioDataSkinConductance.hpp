#pragma once

#include "3rdparty/BioData/src/SkinConductance.h"

#include <halp/audio.hpp>
#include <halp/controls.hpp>
#include <halp/meta.hpp>
#include <puara/gestures.h>

namespace puara_gestures::objects
{
class BioData_Skin_Conductance
{
public:
  halp_meta(name, "Skin conductance")
  halp_meta(category, "BioData")
  halp_meta(c_name, "BioData_Skin_Conductance")
  halp_meta(
      author,
      "BioData authors, Erin Gee, Martin Peach, Thomas Ouellet Fredericks, Sofian "
      "Audry, Luana Belinsky")
  halp_meta(manual_url, "https://github.com/eringee/BioData")
  halp_meta(uuid, "e0cc62f9-eb49-4ff1-b11c-0233d4ed94cf")

  struct ins
  {
    halp::val_port<"Skin_conductance_signal", float> sc_signal;
  } inputs;

  struct
  {
    halp::val_port<"Skin_conductance_raw", float> sc_raw;
    halp::val_port<"Skin_conductance_scr", float> sc_scr;
    halp::val_port<"Skin_conductance_scl", float> sc_scl;
  } outputs;

  halp::setup setup;
  void prepare(halp::setup info) { setup = info; }

  using tick = halp::tick;
  void operator()(halp::tick t);

  SkinConductance sc;
};
}
