cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

include(FetchContent)
FetchContent_Declare(xtl GIT_REPOSITORY https://github.com/xtensor-stack/xtl.git GIT_TAG 0.7.5)
FetchContent_Declare(xsimd GIT_REPOSITORY https://github.com/xtensor-stack/xsimd.git GIT_TAG 9.0.1)
FetchContent_Declare(xtensor GIT_REPOSITORY https://github.com/xtensor-stack/xtensor.git GIT_TAG 0.24.6)
FetchContent_MakeAvailable(xtl xsimd xtensor)

if(NOT TARGET score_lib_base)
  include(ScoreExternalAddon)
endif()

if(NOT TARGET score_plugin_avnd)
  return()
endif()

project(score_addon_puara LANGUAGES CXX)

set(PUARA_GESTURES_ENABLE_TESTING OFF)
set(PUARA_GESTURES_ENABLE_STANDALONE OFF)

add_subdirectory(3rdparty/BioData SYSTEM)
target_compile_features(BioData PRIVATE cxx_std_23)
add_subdirectory(3rdparty/extras SYSTEM)
target_compile_features(extras PRIVATE cxx_std_23)

avnd_score_plugin_init(
  BASE_TARGET score_addon_puara
)

target_sources(score_addon_puara
  PRIVATE

  3rdparty/puara-gestures/3rdparty/IMU_Sensor_Fusion/imu_orientation.h
  3rdparty/puara-gestures/3rdparty/IMU_Sensor_Fusion/imu_orientation.cpp

  3rdparty/puara-gestures/include/puara/descriptors/button.h
  3rdparty/puara-gestures/include/puara/descriptors/jab.h
  3rdparty/puara-gestures/include/puara/descriptors/roll.h
  3rdparty/puara-gestures/include/puara/descriptors/shake.h
  3rdparty/puara-gestures/include/puara/descriptors/tilt.h
  3rdparty/puara-gestures/include/puara/descriptors/touchArrayGestureDetector.h

  3rdparty/puara-gestures/include/puara/utils/blobDetector.h
  3rdparty/puara-gestures/include/puara/utils/calibration.h
  3rdparty/puara-gestures/include/puara/utils/circularbuffer.h
  3rdparty/puara-gestures/include/puara/utils/leakyintegrator.h
  3rdparty/puara-gestures/include/puara/utils/maprange.h
  3rdparty/puara-gestures/include/puara/utils/rollingminmax.h
  3rdparty/puara-gestures/include/puara/utils/smooth.h
  3rdparty/puara-gestures/include/puara/utils/threshold.h
  3rdparty/puara-gestures/include/puara/utils/wrap.h

  3rdparty/puara-gestures/include/puara/structs.h
  3rdparty/puara-gestures/include/puara/gestures.h
  3rdparty/puara-gestures/include/puara/utils.h

  3rdparty/puara-gestures/src/puara_gestures.cpp

  3rdparty/extras/PeakDetector.h
  3rdparty/extras/PeakDetector.cpp
  3rdparty/extras/helpers.h
  3rdparty/extras/helpers.cpp
)

target_include_directories(score_addon_puara
  PRIVATE
  3rdparty/puara-gestures/include/
  3rdparty/puara-gestures/3rdparty/
)
target_link_libraries(score_addon_puara PUBLIC BioData xtensor)

avnd_score_plugin_add(
  BASE_TARGET score_addon_puara
  SOURCES
    Puara/GestureRecognizer.hpp
    Puara/GestureRecognizer.cpp
  TARGET puara_gesture
  MAIN_CLASS GestureRecognizer
  NAMESPACE puara_gestures::objects
)

avnd_score_plugin_add(
  BASE_TARGET score_addon_puara
  SOURCES
    Puara/LeakyIntegratorAvnd.hpp
    Puara/LeakyIntegratorAvnd.cpp
  TARGET puara_leaky_integrator
  MAIN_CLASS LeakyIntegratorAvnd
  NAMESPACE puara_gestures::objects
)

avnd_score_plugin_add(
  BASE_TARGET score_addon_puara
  SOURCES
    Puara/BioDataHeart.hpp
    Puara/BioDataHeart.cpp
  TARGET puara_heart
  MAIN_CLASS BioData_Heart
  NAMESPACE puara_gestures::objects
)

avnd_score_plugin_add(
  BASE_TARGET score_addon_puara
  SOURCES
    Puara/PowerBandAvnd.hpp
    Puara/PowerBandAvnd.cpp
  TARGET puara_powerband
  MAIN_CLASS PowerBandAvnd
  NAMESPACE puara_gestures::objects
)


avnd_score_plugin_add(
  BASE_TARGET score_addon_puara
  SOURCES
    Puara/ButtonAvnd.hpp
    Puara/ButtonAvnd.cpp
  TARGET puara_button_avnd
  MAIN_CLASS ButtonAvnd
  NAMESPACE puara_gestures::objects
)

avnd_score_plugin_add(
  BASE_TARGET score_addon_puara
  SOURCES
    Puara/BioDataSkinConductance.hpp
    Puara/BioDataSkinConductance.cpp
  TARGET puara_skin_conductance
  MAIN_CLASS BioData_Skin_Conductance
  NAMESPACE puara_gestures::objects
)

avnd_score_plugin_add(
  BASE_TARGET score_addon_puara
  SOURCES
    Puara/PeakDetection.hpp
    Puara/PeakDetection.cpp
    3rdparty/extras/PeakDetector.cpp
    3rdparty/extras/PeakDetector.h
  TARGET puara_peak_detection
  MAIN_CLASS PeakDetection
  NAMESPACE puara_gestures::objects
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
  TARGET puara_jab_1d
  MAIN_CLASS Jab1D_Avnd
  NAMESPACE puara_gestures::objects
)


avnd_score_plugin_add(
  BASE_TARGET score_addon_puara
  SOURCES
    Puara/Jab3D_Avnd.hpp
    Puara/Jab3D_Avnd.cpp
  TARGET puara_jab_3d_avnd
  MAIN_CLASS Jab3D_Avnd
  NAMESPACE puara_gestures::objects
)


avnd_score_plugin_add(
  BASE_TARGET score_addon_puara
  SOURCES
    Puara/Jab2D_Avnd.hpp
    Puara/Jab2D_Avnd.cpp
  TARGET puara_jab_2d_avnd
  MAIN_CLASS Jab2D_Avnd
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

avnd_score_plugin_finalize(
  BASE_TARGET score_addon_puara
  PLUGIN_VERSION 1
  PLUGIN_UUID "049a40e0-01d9-4040-9d00-21f6931c3035"
)
