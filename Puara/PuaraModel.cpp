#include "Puara.hpp"
namespace Example
{
void Puara::operator()(halp::tick t)
{
  // 12
  outputs.output = r.roll(inputs.accel.value, inputs.gyro.value, inputs.mag.value, 0.1);
}
}
