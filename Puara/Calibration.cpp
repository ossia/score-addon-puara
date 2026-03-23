#include "Calibration.hpp"

namespace puara_gestures::objects
{

void Calibration::operator()(tick t)
{
  if (inputs.record_data.value != m_last_record_state)
  {
    m_is_recording = inputs.record_data.value;
    if (m_is_recording)
    {
      m_calibration_logic.rawMagData.clear();
      outputs.points_collected.value = 0;
    }
    m_last_record_state = m_is_recording;
  }

  if (m_is_recording)
  {
    m_calibration_logic.recordRawMagData(inputs.raw_magnetometer.value);
    outputs.points_collected.value = m_calibration_logic.rawMagData.size();
  }

  if (!inputs.generate_calibration.values.empty())
  {
    if(m_is_recording)
    {
      m_is_recording = false;
      inputs.record_data.value = false;
      m_last_record_state = false;
    }

    if (!m_calibration_logic.rawMagData.empty())
    {
      m_calibration_logic.gravitationField = inputs.gravitation_field.value;
      m_calibration_logic.generateMagnetometerMatrices();

      const auto& bias = m_calibration_logic.hardIronBias;
      outputs.hard_iron_bias.value = { (float)bias(0), (float)bias(1), (float)bias(2) };

      const auto& matrix = m_calibration_logic.softIronMatrix;
      outputs.soft_iron_row1.value = { (float)matrix(0,0), (float)matrix(0,1), (float)matrix(0,2) };
      outputs.soft_iron_row2.value = { (float)matrix(1,0), (float)matrix(1,1), (float)matrix(1,2) };
      outputs.soft_iron_row3.value = { (float)matrix(2,0), (float)matrix(2,1), (float)matrix(2,2) };
    }
  }

  puara_gestures::Imu9Axis raw_imu;
  raw_imu.magn = inputs.raw_magnetometer.value;

  m_calibration_logic.applyMagnetometerCalibration(raw_imu);

  outputs.calibrated_magnetometer.value = m_calibration_logic.myCalIMU.magn;
}

}
