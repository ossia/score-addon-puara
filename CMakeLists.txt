cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

project(score_addon_puara LANGUAGES CXX)

# Use the Avendish the host (ossia score) provides; otherwise fetch it.
find_package(Avendish QUIET)
if(NOT Avendish_FOUND)
  include(FetchContent)
  FetchContent_Declare(
    Avendish
    GIT_REPOSITORY "https://github.com/celtera/avendish"
    GIT_TAG main)
  FetchContent_Populate(Avendish)
  list(APPEND CMAKE_PREFIX_PATH "${avendish_SOURCE_DIR}")
  find_package(Avendish REQUIRED)
endif()

set(PUARA_GESTURES_ENABLE_TESTING OFF)
set(PUARA_GESTURES_ENABLE_STANDALONE OFF)

if(EXISTS 3rdparty/puara-gestures/3rdparty/boost)
  file(REMOVE_RECURSE 3rdparty/puara-gestures/3rdparty/boost)
endif()

add_subdirectory(3rdparty/BioData SYSTEM)
add_subdirectory(3rdparty/extras SYSTEM)


avnd_addon_init(NAME score_addon_puara)


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

  Puara/halp_utils.hpp
  3rdparty/extras/PeakDetector.h
  3rdparty/extras/PeakDetector.cpp
  3rdparty/extras/Normalizer.h
  3rdparty/extras/Normalizer.cpp
  3rdparty/extras/helpers.h
  3rdparty/extras/helpers.cpp
  3rdparty/extras/EMA.h
  3rdparty/extras/MinMaxScaler.h
  3rdparty/extras/MinMaxScaler.cpp
  3rdparty/extras/QuantileScaler.h
  3rdparty/extras/QuantileScaler.cpp
 
  Puara/vamp_algorithms.hpp
  Puara/statistics_algorithms.hpp
)
target_include_directories(score_addon_puara
  PRIVATE
  3rdparty/puara-gestures/include/
  3rdparty/puara-gestures/3rdparty/
)
target_link_libraries(score_addon_puara PUBLIC BioData)

avnd_addon_object(
  BASE score_addon_puara
  C_NAME puara_gesture
  CLASS GestureRecognizer
  NAMESPACE puara_gestures::objects
  SOURCES
Puara/GestureRecognizer.hpp
    Puara/GestureRecognizer.cpp)
avnd_addon_object(
  BASE score_addon_puara
  C_NAME puara_powerband
  CLASS PowerBandAvnd
  NAMESPACE puara_gestures::objects
  SOURCES
Puara/PowerBandAvnd.hpp
    Puara/PowerBandAvnd.cpp)

avnd_addon_object(
  BASE score_addon_puara
  C_NAME puara_erp_avnd
  CLASS ERPAvnd
  NAMESPACE puara_gestures::objects
  SOURCES
Puara/ERPAvnd.hpp
    Puara/ERPAvnd.cpp)
avnd_addon_object(
  BASE score_addon_puara
  C_NAME puara_correlation_avnd
  CLASS CorrelationAvnd
  NAMESPACE puara_gestures::objects
  SOURCES
Puara/CorrelationAvnd.hpp
    Puara/CorrelationAvnd.cpp)

avnd_addon_object(
  BASE score_addon_puara
  C_NAME puara_eda
  CLASS EdaRtFeatures
  NAMESPACE puara_gestures::objects
  SOURCES
Puara/EdaRtFeatures.hpp
    Puara/EdaRtFeatures.cpp)

avnd_addon_object(
  BASE score_addon_puara
  C_NAME puara_heart
  CLASS BioData_Heart
  NAMESPACE puara_gestures::objects
  SOURCES
Puara/BioDataHeart.hpp
    Puara/BioDataHeart.cpp)

avnd_addon_object(
  BASE score_addon_puara
  C_NAME puara_scaler
  CLASS Scaler
  NAMESPACE puara_gestures::objects
  SOURCES
Puara/Scaler.hpp
    Puara/Scaler.cpp)

avnd_addon_object(
  BASE score_addon_puara
  C_NAME puara_normalization
  CLASS Normalization
  NAMESPACE puara_gestures::objects
  SOURCES
Puara/Normalization.hpp
    Puara/Normalization.cpp)

avnd_addon_object(
  BASE score_addon_puara
  C_NAME smoother
  CLASS Smoother
  NAMESPACE puara_gestures::objects
  SOURCES
Puara/Smoother.hpp
    Puara/Smoother.cpp)


avnd_addon_object(
  BASE score_addon_puara
  C_NAME puara_button_avnd
  CLASS ButtonAvnd
  NAMESPACE puara_gestures::objects
  SOURCES
Puara/ButtonAvnd.hpp
    Puara/ButtonAvnd.cpp)

avnd_addon_object(
  BASE score_addon_puara
  C_NAME walker_avnd
  CLASS WalkerAvnd
  NAMESPACE puara_gestures::objects
  SOURCES
Puara/WalkerAvnd.hpp
    Puara/WalkerAvnd.cpp)


avnd_addon_object(
  BASE score_addon_puara
  C_NAME puara_skin_conductance
  CLASS BioData_Skin_Conductance
  NAMESPACE puara_gestures::objects
  SOURCES
Puara/BioDataSkinConductance.hpp
    Puara/BioDataSkinConductance.cpp)
avnd_addon_object(
  BASE score_addon_puara
  C_NAME ClusteringAvnd
  CLASS ClusteringAvnd
  NAMESPACE puara_gestures::objects
  SOURCES
Puara/ClusteringAvnd.hpp
    Puara/ClusteringAvnd.cpp)
avnd_addon_object(
  BASE score_addon_puara
  C_NAME vamp_avnd
  CLASS VAMPAvnd
  NAMESPACE puara_gestures::objects
  SOURCES
Puara/VAMPAvnd.hpp
    Puara/VAMPAvnd.cpp)

avnd_addon_object(
  BASE score_addon_puara
  C_NAME puara_peak_detection
  CLASS PeakDetection
  NAMESPACE puara_gestures::objects
  SOURCES
Puara/PeakDetection.hpp
    Puara/PeakDetection.cpp
    3rdparty/extras/PeakDetector.cpp
    3rdparty/extras/PeakDetector.h)

avnd_addon_object(
  BASE score_addon_puara
  C_NAME puara_rate_of_change
  CLASS RateOfChange
  NAMESPACE puara_gestures::objects
  SOURCES
Puara/RateOfChange.hpp
    Puara/RateOfChange.cpp)


avnd_addon_object(
  BASE score_addon_puara
  C_NAME puara_roll
  CLASS Roll
  NAMESPACE puara_gestures::objects
  SOURCES
Puara/Roll.hpp
    Puara/Roll.cpp)

avnd_addon_object(
  BASE score_addon_puara
  C_NAME puara_tilt
  CLASS Tilt
  NAMESPACE puara_gestures::objects
  SOURCES
Puara/Tilt.hpp
    Puara/Tilt.cpp)
avnd_addon_object(
  BASE score_addon_puara
  C_NAME puara_jab_1d
  CLASS Jab1D_Avnd
  NAMESPACE puara_gestures::objects
  SOURCES
Puara/Jab.hpp
    Puara/Jab.cpp)



avnd_addon_object(
  BASE score_addon_puara
  C_NAME puara_binarizer
  CLASS Binarizer
  NAMESPACE puara_gestures::objects
  SOURCES
Puara/Binarizer.hpp
    Puara/Binarizer.cpp)
avnd_addon_object(
  BASE score_addon_puara
  C_NAME puara_jab_3d_avnd
  CLASS Jab3D_Avnd
  NAMESPACE puara_gestures::objects
  SOURCES
Puara/Jab3D_Avnd.hpp
    Puara/Jab3D_Avnd.cpp)
avnd_addon_object(
  BASE score_addon_puara
  C_NAME puara_compass_avnd
  CLASS CompassAvnd
  NAMESPACE puara_gestures::objects
  SOURCES
Puara/CompassAvnd.hpp
    Puara/CompassAvnd.cpp)

avnd_addon_object(
  BASE score_addon_puara
  C_NAME pca_avnd
  CLASS PCAAvnd
  NAMESPACE puara_gestures::objects
  SOURCES
Puara/PCAAvnd.hpp
    Puara/PCAAvnd.cpp)
avnd_addon_object(
  BASE score_addon_puara
  C_NAME puara_powerband_eeg
  CLASS PowerBandEEGAvnd
  NAMESPACE puara_gestures::objects
  SOURCES
Puara/PowerBandEEGAvnd.hpp
    Puara/PowerBandEEGAvnd.cpp)

avnd_addon_object(
  BASE score_addon_puara
  C_NAME puara_jab_2d_avnd
  CLASS Jab2D_Avnd
  NAMESPACE puara_gestures::objects
  SOURCES
Puara/Jab2D_Avnd.hpp
    Puara/Jab2D_Avnd.cpp)
avnd_addon_object(
  BASE score_addon_puara
  C_NAME puara_shake
  CLASS Shake
  NAMESPACE puara_gestures::objects
  SOURCES
Puara/Shake.hpp
    Puara/Shake.cpp)

avnd_addon_finalize(NAME score_addon_puara UUID 049a40e0-01d9-4040-9d00-21f6931c3035 VERSION 1)
