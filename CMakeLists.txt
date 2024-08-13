cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

if(NOT TARGET score_lib_base)
  include(ScoreExternalAddon)
endif()

if(NOT TARGET score_plugin_avnd)
  return()
endif()

project(score_addon_puara LANGUAGES CXX)

avnd_score_plugin_init(
  BASE_TARGET score_addon_puara
)

avnd_score_plugin_add(
  BASE_TARGET score_addon_puara
  SOURCES
    Puara/Puara.hpp
    Puara/PuaraModel.hpp
    Puara/PuaraModel.cpp
    Puara/PuaraUi.hpp
    3rdparty/puara-gestures/descriptors/IMU_Sensor_Fusion/imu_orientation.cpp
  TARGET puara
  MAIN_CLASS Puara
  NAMESPACE Example
)

target_include_directories(score_addon_puara
PRIVATE 3rdparty/puara-gestures 3rdparty/puara-gestures/descriptors)

avnd_score_plugin_finalize(
  BASE_TARGET score_addon_puara
  PLUGIN_VERSION 1
  PLUGIN_UUID "049a40e0-01d9-4040-9d00-21f6931c3035"
)
