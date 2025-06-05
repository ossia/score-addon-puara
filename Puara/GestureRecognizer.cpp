#include "GestureRecognizer.hpp"

#include <iostream>

namespace puara_gestures::objects
{
void GestureRecognizer::operator()(halp::tick t)
{
  const double period = t.frames / setup.rate;

  auto [ax, ay, az] = inputs.accel.value;
  jab.update(ax, ay, az);
  shake.update(ax, ay, az);
  heart.update(inputs.heart_signal);
  gsr.update(inputs.GSR_signal);

  outputs.jab = jab.current_value();
  outputs.shake = shake.current_value();
  outputs.tilt = tilt.tilt(inputs.accel, inputs.gyro, inputs.mag, period);
  outputs.roll = roll.roll(inputs.accel, inputs.gyro, inputs.mag, period);

  outputs.heart_raw = heart.getRaw();
  outputs.heart_normalized = heart.getNormalized();
  outputs.heart_beat = heart.beatDetected();
  outputs.heart_bpm = heart.getBPM();
  outputs.heart_bpmChange = heart.bpmChange();
  outputs.heart_amplitudeChange = heart.amplitudeChange();

  outputs.GSR_raw = gsr.getRaw();
  outputs.GSR_SCR = gsr.getSCR();
  outputs.GSR_SCL = gsr.getSCL();
}
}
