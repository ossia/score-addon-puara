#include "BioDataSkinConductance.hpp"

namespace puara_gestures::objects
{
void BioData_Skin_Conductance::operator()(halp::tick t)
{
  const double period = t.frames / setup.rate;
  
  sc.update(inputs.sc_signal);

  outputs.sc_raw = sc.getRaw();
  outputs.sc_scr = sc.getSCR();
  outputs.sc_scl = sc.getSCL();
}
}
