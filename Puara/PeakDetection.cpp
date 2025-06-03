#include "PeakDetection.hpp"

namespace puara_gestures::objects
{

PeakDetector detector(0.5, PEAK_MAX);

void PeakDetection::prepare(halp::setup info)
{
  detector.reloadThreshold(0.2);
  detector.fallbackTolerance(0.1);
  setup = info;
};

void PeakDetection::operator()(halp::tick t)
{
  const double period = t.frames / setup.rate;
  // time since last called
  //TODO: works with toggle but otherwise loses some peaks
  //(keep value until next ossia sampling?) is it the value display?

  detector.mode(inputs.mode.value);

  float value = inputs.peakDetection_signal;
  bool peakDetected = detector.put(value);

  if(peakDetected)
  {
    toggle = !toggle;
  }

  outputs.peak = toggle;
}
}
