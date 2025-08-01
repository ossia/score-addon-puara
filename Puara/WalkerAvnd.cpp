#include "WalkerAvnd.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace puara_gestures::objects
{

WalkerAvnd::WalkerAvnd()
{
  reset_position();
}

void WalkerAvnd::operator()()
{
  if(inputs.reset.value.has_value())
  {
    reset_position();
  }

  double angle_deg = inputs.angle.value;
  double velocity = inputs.velocity.value;
  bool is_on_water = inputs.on_water.value;

  if(is_on_water)
  {
    velocity *= inputs.water_speed_factor.value;
  }

  const double angle_rad = angle_deg * M_PI / 180.0;

  double lat_change = velocity * std::cos(angle_rad);
  double lon_change = velocity * std::sin(angle_rad);

  if(std::abs(m_current_latitude) < 89.9)
  {
    lon_change /= std::cos(m_current_latitude * M_PI / 180.0);
  }
  m_current_latitude += lat_change;
  m_current_longitude += lon_change;

  if(m_current_latitude > 90.0)
  {
    // Crossed north pole - reflect latitude and flip longitude
    m_current_latitude = 180.0 - m_current_latitude;
    m_current_longitude += 180.0;
  }
  else if(m_current_latitude < -90.0)
  {
    // Crossed south pole - reflect latitude and flip longitude
    m_current_latitude = -180.0 - m_current_latitude;
    m_current_longitude += 180.0;
  }

  // Wrap longitude to [-180, 180] range
  while(m_current_longitude > 180.0)
    m_current_longitude -= 360.0;
  while(m_current_longitude < -180.0)
    m_current_longitude += 360.0;
  outputs.latitude.value = m_current_latitude;
  outputs.longitude.value = m_current_longitude;
}

void WalkerAvnd::reset_position()
{
  m_current_latitude = inputs.initial_latitude.value;
  m_current_longitude = inputs.initial_longitude.value;
}

}
