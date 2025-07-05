#include "BioDataHeart.hpp"

namespace puara_gestures::objects
{
void BioData_Heart::operator()(halp::tick_musical t)
{
  heart.update(
      inputs.heart_signal, 1e6 * t.position_in_frames / double(this->setup.rate));

  outputs.heart_raw = heart.getRaw();
  outputs.heart_normalized = heart.getNormalized();
  outputs.heart_beat = heart.beatDetected();
  outputs.heart_bpm = heart.getBPM();
  outputs.heart_bpmChange = heart.bpmChange();
  outputs.heart_amplitudeChange = heart.amplitudeChange();
}
}
