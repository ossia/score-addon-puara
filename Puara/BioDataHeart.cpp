#include "BioDataHeart.hpp"

namespace puara_gestures::objects
{
void BioData_Heart::operator()(halp::tick t)
{
  const double period = t.frames / setup.rate;

  heart.update(inputs.heart_signal);

  outputs.heart_raw = heart.getRaw();
  outputs.heart_normalized = heart.getNormalized();
  outputs.heart_beat = heart.beatDetected();
  outputs.heart_bpm = heart.getBPM();
  outputs.heart_bpmChange = heart.bpmChange();
  outputs.heart_amplitudeChange = heart.amplitudeChange();
}
}
