cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

if(NOT TARGET score_lib_base)
  include(ScoreExternalAddon)
endif()

if(NOT TARGET score_plugin_avnd)
  return()
endif()

project(score_addon_puara LANGUAGES CXX)

set(PUARA_GESTURES_ENABLE_TESTING OFF)
set(PUARA_GESTURES_ENABLE_STANDALONE OFF)

if(EXISTS 3rdparty/puara-gestures/3rdparty/boost)
  file(REMOVE_RECURSE 3rdparty/puara-gestures/3rdparty/boost)
endif()

add_subdirectory(3rdparty/BioData SYSTEM)
add_subdirectory(3rdparty/extras SYSTEM)


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
  3rdparty/extras/EMA.h
  Puara/vamp_algorithms.hpp
  Puara/statistics_algorithms.hpp
)
target_include_directories(score_addon_puara
  PRIVATE
  3rdparty/puara-gestures/include/
  3rdparty/puara-gestures/3rdparty/
)
target_link_libraries(score_addon_puara PUBLIC BioData)

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
    Puara/PowerBandAvnd.hpp
    Puara/PowerBandAvnd.cpp
  TARGET puara_powerband
  MAIN_CLASS PowerBandAvnd
  NAMESPACE puara_gestures::objects
)

avnd_score_plugin_add(
  BASE_TARGET score_addon_puara
  SOURCES
    Puara/ERPAvnd.hpp
    Puara/ERPAvnd.cpp
  TARGET puara_erp_avnd
  MAIN_CLASS ERPAvnd
  NAMESPACE puara_gestures::objects
)
avnd_score_plugin_add(
  BASE_TARGET score_addon_puara
  SOURCES
    Puara/CorrelationAvnd.hpp
    Puara/CorrelationAvnd.cpp
  TARGET puara_correlation_avnd
  MAIN_CLASS CorrelationAvnd
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
    Puara/Smoother.hpp
    Puara/Smoother.cpp
  TARGET smoother
  MAIN_CLASS Smoother
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
    Puara/WalkerAvnd.hpp
    Puara/WalkerAvnd.cpp
  TARGET walker_avnd
  MAIN_CLASS WalkerAvnd
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
    Puara/ClusteringAvnd.hpp
    Puara/ClusteringAvnd.cpp
  TARGET ClusteringAvnd
  MAIN_CLASS ClusteringAvnd
  NAMESPACE puara_gestures::objects
)
avnd_score_plugin_add(
  BASE_TARGET score_addon_puara
  SOURCES
    Puara/VAMPAvnd.hpp
    Puara/VAMPAvnd.cpp
  TARGET vamp_avnd
  MAIN_CLASS VAMPAvnd
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
    Puara/Binarizer.hpp
    Puara/Binarizer.cpp
  TARGET puara_binarizer
  MAIN_CLASS Binarizer
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
    Puara/CompassAvnd.hpp
    Puara/CompassAvnd.cpp
  TARGET puara_compass_avnd
  MAIN_CLASS CompassAvnd
  NAMESPACE puara_gestures::objects
)

avnd_score_plugin_add(
  BASE_TARGET score_addon_puara
  SOURCES
    Puara/PCAAvnd.hpp
    Puara/PCAAvnd.cpp
  TARGET pca_avnd
  MAIN_CLASS PCAAvnd
  NAMESPACE puara_gestures::objects
)
avnd_score_plugin_add(
  BASE_TARGET score_addon_puara
  SOURCES
    Puara/PowerBandEEGAvnd.hpp
    Puara/PowerBandEEGAvnd.cpp
  TARGET puara_powerband_eeg
  MAIN_CLASS PowerBandEEGAvnd
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
