#include "WalkerAvnd.hpp"

namespace puara_gestures::objects
{

WalkerAvnd::WalkerAvnd()
{
  reset_position();
}

void WalkerAvnd::operator()()
{
  if(!params.reset.value.has_value())
  {
    reset_position();
  }
  double angle_deg = inputs.angle.value;
  double velocity = inputs.velocity.value;
  bool is_on_water = inputs.on_water.value;
  if(is_on_water)
  {
    velocity *= params.water_speed_factor.value;
  }
  const double angle_rad = angle_deg * ossia::pi / 180.0;
  m_current_latitude += velocity * std::cos(angle_rad);
  m_current_longitude += velocity * std::sin(angle_rad);
  if(m_current_latitude > 90.0)
  {
    m_current_latitude = 180.0 - m_current_latitude;
  }
  else if(m_current_latitude < -90.0)
  {
    m_current_latitude = -180.0 - m_current_latitude;
  }
  m_current_longitude = std::fmod(m_current_longitude + 180.0, 360.0) - 180.0;
  if(m_current_longitude == -180.0)
    m_current_longitude = 180.0;
  outputs.latitude.value = m_current_latitude;
  outputs.longitude.value = m_current_longitude;
}
void WalkerAvnd::reset_position()
{
  m_current_latitude = params.initial_latitude.value;
  m_current_longitude = params.initial_longitude.value;
}

}
