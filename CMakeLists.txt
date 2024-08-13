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

target_sources(score_addon_puara PRIVATE
    3rdparty/puara-gestures/descriptors/IMU_Sensor_Fusion/imu_orientation.cpp
#    3rdparty/puara-gestures/descriptors/puara-utils.cpp
)

avnd_score_plugin_add(
  BASE_TARGET score_addon_puara
  SOURCES
    Puara/LeakyIntegrator.hpp
  TARGET puara
  MAIN_CLASS LeakyIntegrator
  NAMESPACE Puara
)

avnd_score_plugin_add(
  BASE_TARGET score_addon_puara
  SOURCES
    Puara/Roll.hpp
    Puara/Roll.cpp
  TARGET puara_roll
  MAIN_CLASS Roll
  NAMESPACE puara_gestures::objects
)

avnd_score_plugin_add(
  BASE_TARGET score_addon_puara
  SOURCES
    Puara/Tilt.hpp
    Puara/Tilt.cpp
  TARGET puara_tilt
  MAIN_CLASS Tilt
  NAMESPACE puara_gestures::objects
)

avnd_score_plugin_add(
  BASE_TARGET score_addon_puara
  SOURCES
    Puara/Jab.hpp
    Puara/Jab.cpp
  TARGET puara_jab
  MAIN_CLASS Jab
  NAMESPACE puara_gestures::objects
)

avnd_score_plugin_add(
  BASE_TARGET score_addon_puara
  SOURCES
    Puara/Shake.hpp
    Puara/Shake.cpp
  TARGET puara_shake
  MAIN_CLASS Shake
  NAMESPACE puara_gestures::objects
)

target_include_directories(score_addon_puara
  PRIVATE
    3rdparty/puara-gestures
    3rdparty/puara-gestures/descriptors
)

avnd_score_plugin_finalize(
  BASE_TARGET score_addon_puara
  PLUGIN_VERSION 1
  PLUGIN_UUID "049a40e0-01d9-4040-9d00-21f6931c3035"
)
