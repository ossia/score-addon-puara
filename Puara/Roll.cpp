#include "Roll.hpp"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


namespace puara_gestures::objects
{
void Roll::operator()(halp::tick t)
{

  double period_s = 0.0;
  if(setup.frames>0&&t.frames>0){
    period_s = static_cast<double>(t.frames)/setup.rate;

  }else{}

  //2. get sensor input
  const puara_gestures::Coord3D& current_accel = inputs.accel.value;
  const puara_gestures::Coord3D& current_gyro  = inputs.gyro.value;
  const puara_gestures::Coord3D& current_mag   = inputs.mag.value;

  //3. getting  post-Processing Enable Flags
  const bool unwrap_enabled = inputs.enable_unwrap.value;
  const bool smooth_enabled = inputs.enable_smooth.value;
  const bool wrap_enabled   = inputs.enable_wrap.value;

  //4. calling roll calc -- gets raw roll from imu orientation
  //note puara_gestures::Roll::roll returns a double
  double current_roll_rad = impl.roll(current_accel,current_gyro,current_mag,period_s);

  //5. applying the post processing stages
  if(unwrap_enabled){
    current_roll_rad = impl.unwrap(current_roll_rad);
  }
  if(smooth_enabled){
    current_roll_rad = impl.smooth(current_roll_rad);
  }
  if(wrap_enabled){
    current_roll_rad = impl.wrap(current_roll_rad);
  }

  //6. setting the output port
  outputs.output.value = static_cast<float>(current_roll_rad);


}
}
