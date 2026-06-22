cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

project(score_addon_puara LANGUAGES CXX)

# Max/MSP externals link the static CRT (/MT) on Windows, as the official
# max-sdk-base requires. Set it for the whole addon -- before Avendish and the
# object library are created -- so every target shares one runtime; mixing /MT
# and /MD trips MSVC with LNK2038. (CMP0091 is NEW via cmake_minimum_required.)
if(MSVC)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# Use the Avendish the host (ossia score) provides; otherwise fetch it.
find_package(Avendish QUIET)
if(NOT Avendish_FOUND)
  include(FetchContent)
  FetchContent_Declare(
    Avendish
    GIT_REPOSITORY "https://github.com/celtera/avendish"
    GIT_TAG ebbbdf81a3c69831c50231877814b63852f9881c)
  FetchContent_Populate(Avendish)
  list(APPEND CMAKE_PREFIX_PATH "${avendish_SOURCE_DIR}")
  find_package(Avendish REQUIRED)
endif()

set(PUARA_GESTURES_ENABLE_TESTING OFF)
set(PUARA_GESTURES_ENABLE_STANDALONE OFF)

# puara-gestures #includes <boost/...> (e.g. boost/circular_buffer.hpp via
# rollingminmax.h). Boost must reach every target that compiles puara-gestures,
# in BOTH build modes:
#   - score mode: ossia score already provides Boost headers on its include path.
#   - standalone / portability: the CI harness fetches a header-only Boost and
#     passes -DBOOST_ROOT=... -DBoost_NO_BOOST_CMAKE=ON, so find_package(Boost)
#     resolves it. The addon then attaches it to the base target, which the
#     unified Avendish API propagates to the objects and every back-end.
# We keep the vendored copy under 3rdparty/puara-gestures/3rdparty/boost as a
# fallback include path (do NOT delete it) so the build still works when no
# external Boost is provided.
find_package(Boost QUIET)

# A handful of objects (PowerBand, ERP, Correlation, PCA, Binarizer, Compass,
# VAMP, PowerBandEEG, statistics/vamp algorithms) need Eigen + xtensor. ossia
# score already provides those targets/headers; standalone does not, so fetch
# the same stack score uses (xtl -> xsimd -> xtensor, plus Eigen3) when absent.
# They are header-only, so this only costs a checkout, not a heavy build.
if(NOT TARGET Eigen3::Eigen OR NOT TARGET xtensor)
  include(FetchContent)
  set(FETCHCONTENT_QUIET OFF)
endif()

if(NOT TARGET Eigen3::Eigen)
  set(EIGEN_BUILD_DOC OFF CACHE BOOL "" FORCE)
  set(EIGEN_BUILD_TESTING OFF CACHE BOOL "" FORCE)
  set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
  FetchContent_Declare(Eigen3
    GIT_REPOSITORY "https://gitlab.com/libeigen/eigen"
    GIT_TAG bc3b39870ecb690a623a3f49149a358b95c5781d
    GIT_SHALLOW TRUE)
  FetchContent_MakeAvailable(Eigen3)
endif()

if(NOT TARGET xtensor)
  FetchContent_Declare(xtl
    GIT_REPOSITORY "https://github.com/xtensor-stack/xtl"
    GIT_TAG c52350e283b98e5d69dbc50726925a1f8e16c57c
    GIT_SHALLOW TRUE)
  FetchContent_MakeAvailable(xtl)
  FetchContent_Declare(xsimd
    GIT_REPOSITORY "https://github.com/xtensor-stack/xsimd"
    GIT_TAG 6842624fc8adafd7168a999e7150b384411da448)
  FetchContent_MakeAvailable(xsimd)
  FetchContent_Declare(xtensor
    GIT_REPOSITORY "https://github.com/xtensor-stack/xtensor"
    GIT_TAG 18f6524829d8ac6399374f9ecbd21b959f75424d)
  FetchContent_MakeAvailable(xtensor)
endif()

add_subdirectory(3rdparty/BioData SYSTEM)
add_subdirectory(3rdparty/extras SYSTEM)

# These static libs are linked into shared modules (pd / max / python / ...) in
# standalone mode, so they must be position-independent. They also use C++17
# features (std::optional, std::clamp) but declare their own project() with no
# standard, so they'd otherwise fall back to the compiler default -- which on
# MSVC is C++14 and breaks the build. Pin them to C++20 like the rest of the addon.
set_target_properties(BioData extras PROPERTIES
  POSITION_INDEPENDENT_CODE ON
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON)


# Standalone back-end set for every object. This is the data-processor preset
# MINUS example_host: puara's objects define a custom tick (has_tick<T> is true),
# which the pinned Avendish example_host binding can't drive (it calls the effect
# with an integral tick and static_asserts !has_tick). Every other object
# back-end builds fine. In a score build BACKENDS is ignored (score introspects
# the object), so this only affects standalone / portability.
set(PUARA_STANDALONE_BACKENDS
  dump ossia python pd max standalone wasm
  touchdesigner:CHOP_MESSAGE godot:NODE)

avnd_addon_init(NAME score_addon_puara)

# Make Boost reach the objects + back-ends through the base target. Only in
# standalone: score already puts its own Boost on the global include path, and we
# must not shadow it with a possibly-different external one. In standalone the
# portability CI provides Boost via BOOST_ROOT; otherwise the vendored headers
# (attached as an include dir below) cover it.
if(NOT AVND_ADDON_SCORE AND Boost_FOUND)
  if(TARGET Boost::headers)
    target_link_libraries(score_addon_puara PUBLIC Boost::headers)
  elseif(TARGET Boost::boost)
    target_link_libraries(score_addon_puara PUBLIC Boost::boost)
  elseif(Boost_INCLUDE_DIRS)
    target_include_directories(score_addon_puara SYSTEM PUBLIC ${Boost_INCLUDE_DIRS})
  endif()
endif()

# Propagate Eigen + xtensor through the base target (objects + back-ends inherit
# it). Only attach in standalone: in score mode these headers are already on the
# global include path and the targets may carry score-specific settings.
if(NOT AVND_ADDON_SCORE)
  if(TARGET Eigen3::Eigen)
    target_link_libraries(score_addon_puara PUBLIC Eigen3::Eigen)
  endif()
  if(TARGET xtensor)
    target_link_libraries(score_addon_puara PUBLIC xtensor)
  endif()
endif()


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
  PUBLIC
  3rdparty/puara-gestures/include/
  # Carries the vendored Boost fallback (3rdparty/.../3rdparty/boost) plus
  # IMU_Sensor_Fusion. PUBLIC so the objects + back-ends that compile
  # puara-gestures headers can resolve <boost/...> when no external Boost exists.
  3rdparty/puara-gestures/3rdparty/
)
target_link_libraries(score_addon_puara PUBLIC BioData)

avnd_addon_object(
  BASE score_addon_puara
  C_NAME puara_gesture
  CLASS GestureRecognizer
  NAMESPACE puara_gestures::objects
  BACKENDS ${PUARA_STANDALONE_BACKENDS}
  SOURCES
Puara/GestureRecognizer.hpp
    Puara/GestureRecognizer.cpp)
avnd_addon_object(
  BASE score_addon_puara
  C_NAME puara_powerband
  CLASS PowerBandAvnd
  NAMESPACE puara_gestures::objects
  BACKENDS ${PUARA_STANDALONE_BACKENDS}
  SOURCES
Puara/PowerBandAvnd.hpp
    Puara/PowerBandAvnd.cpp)

avnd_addon_object(
  BASE score_addon_puara
  C_NAME puara_erp_avnd
  CLASS ERPAvnd
  NAMESPACE puara_gestures::objects
  BACKENDS ${PUARA_STANDALONE_BACKENDS}
  SOURCES
Puara/ERPAvnd.hpp
    Puara/ERPAvnd.cpp)
avnd_addon_object(
  BASE score_addon_puara
  C_NAME puara_correlation_avnd
  CLASS CorrelationAvnd
  NAMESPACE puara_gestures::objects
  BACKENDS ${PUARA_STANDALONE_BACKENDS}
  SOURCES
Puara/CorrelationAvnd.hpp
    Puara/CorrelationAvnd.cpp)

avnd_addon_object(
  BASE score_addon_puara
  C_NAME puara_eda
  CLASS EdaRtFeatures
  NAMESPACE puara_gestures::objects
  BACKENDS ${PUARA_STANDALONE_BACKENDS}
  SOURCES
Puara/EdaRtFeatures.hpp
    Puara/EdaRtFeatures.cpp)

avnd_addon_object(
  BASE score_addon_puara
  C_NAME puara_heart
  CLASS BioData_Heart
  NAMESPACE puara_gestures::objects
  BACKENDS ${PUARA_STANDALONE_BACKENDS}
  SOURCES
Puara/BioDataHeart.hpp
    Puara/BioDataHeart.cpp)

avnd_addon_object(
  BASE score_addon_puara
  C_NAME puara_scaler
  CLASS Scaler
  NAMESPACE puara_gestures::objects
  BACKENDS ${PUARA_STANDALONE_BACKENDS}
  SOURCES
Puara/Scaler.hpp
    Puara/Scaler.cpp)

avnd_addon_object(
  BASE score_addon_puara
  C_NAME puara_normalization
  CLASS Normalization
  NAMESPACE puara_gestures::objects
  BACKENDS ${PUARA_STANDALONE_BACKENDS}
  SOURCES
Puara/Normalization.hpp
    Puara/Normalization.cpp)

avnd_addon_object(
  BASE score_addon_puara
  C_NAME smoother
  CLASS Smoother
  NAMESPACE puara_gestures::objects
  BACKENDS ${PUARA_STANDALONE_BACKENDS}
  SOURCES
Puara/Smoother.hpp
    Puara/Smoother.cpp)


avnd_addon_object(
  BASE score_addon_puara
  C_NAME puara_button_avnd
  CLASS ButtonAvnd
  NAMESPACE puara_gestures::objects
  BACKENDS ${PUARA_STANDALONE_BACKENDS}
  SOURCES
Puara/ButtonAvnd.hpp
    Puara/ButtonAvnd.cpp)

avnd_addon_object(
  BASE score_addon_puara
  C_NAME walker_avnd
  CLASS WalkerAvnd
  NAMESPACE puara_gestures::objects
  BACKENDS ${PUARA_STANDALONE_BACKENDS}
  SOURCES
Puara/WalkerAvnd.hpp
    Puara/WalkerAvnd.cpp)


avnd_addon_object(
  BASE score_addon_puara
  C_NAME puara_skin_conductance
  CLASS BioData_Skin_Conductance
  NAMESPACE puara_gestures::objects
  BACKENDS ${PUARA_STANDALONE_BACKENDS}
  SOURCES
Puara/BioDataSkinConductance.hpp
    Puara/BioDataSkinConductance.cpp)
avnd_addon_object(
  BASE score_addon_puara
  C_NAME ClusteringAvnd
  CLASS ClusteringAvnd
  NAMESPACE puara_gestures::objects
  BACKENDS ${PUARA_STANDALONE_BACKENDS}
  SOURCES
Puara/ClusteringAvnd.hpp
    Puara/ClusteringAvnd.cpp)
avnd_addon_object(
  BASE score_addon_puara
  C_NAME vamp_avnd
  CLASS VAMPAvnd
  NAMESPACE puara_gestures::objects
  BACKENDS ${PUARA_STANDALONE_BACKENDS}
  SOURCES
Puara/VAMPAvnd.hpp
    Puara/VAMPAvnd.cpp)

avnd_addon_object(
  BASE score_addon_puara
  C_NAME puara_peak_detection
  CLASS PeakDetection
  NAMESPACE puara_gestures::objects
  BACKENDS ${PUARA_STANDALONE_BACKENDS}
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
  BACKENDS ${PUARA_STANDALONE_BACKENDS}
  SOURCES
Puara/RateOfChange.hpp
    Puara/RateOfChange.cpp)


avnd_addon_object(
  BASE score_addon_puara
  C_NAME puara_roll
  CLASS Roll
  NAMESPACE puara_gestures::objects
  BACKENDS ${PUARA_STANDALONE_BACKENDS}
  SOURCES
Puara/Roll.hpp
    Puara/Roll.cpp)

avnd_addon_object(
  BASE score_addon_puara
  C_NAME puara_tilt
  CLASS Tilt
  NAMESPACE puara_gestures::objects
  BACKENDS ${PUARA_STANDALONE_BACKENDS}
  SOURCES
Puara/Tilt.hpp
    Puara/Tilt.cpp)
avnd_addon_object(
  BASE score_addon_puara
  C_NAME puara_jab_1d
  CLASS Jab1D_Avnd
  NAMESPACE puara_gestures::objects
  BACKENDS ${PUARA_STANDALONE_BACKENDS}
  SOURCES
Puara/Jab.hpp
    Puara/Jab.cpp)



avnd_addon_object(
  BASE score_addon_puara
  C_NAME puara_binarizer
  CLASS Binarizer
  NAMESPACE puara_gestures::objects
  BACKENDS ${PUARA_STANDALONE_BACKENDS}
  SOURCES
Puara/Binarizer.hpp
    Puara/Binarizer.cpp)
avnd_addon_object(
  BASE score_addon_puara
  C_NAME puara_jab_3d_avnd
  CLASS Jab3D_Avnd
  NAMESPACE puara_gestures::objects
  BACKENDS ${PUARA_STANDALONE_BACKENDS}
  SOURCES
Puara/Jab3D_Avnd.hpp
    Puara/Jab3D_Avnd.cpp)
avnd_addon_object(
  BASE score_addon_puara
  C_NAME puara_compass_avnd
  CLASS CompassAvnd
  NAMESPACE puara_gestures::objects
  BACKENDS ${PUARA_STANDALONE_BACKENDS}
  SOURCES
Puara/CompassAvnd.hpp
    Puara/CompassAvnd.cpp)

avnd_addon_object(
  BASE score_addon_puara
  C_NAME pca_avnd
  CLASS PCAAvnd
  NAMESPACE puara_gestures::objects
  BACKENDS ${PUARA_STANDALONE_BACKENDS}
  SOURCES
Puara/PCAAvnd.hpp
    Puara/PCAAvnd.cpp)
avnd_addon_object(
  BASE score_addon_puara
  C_NAME puara_powerband_eeg
  CLASS PowerBandEEGAvnd
  NAMESPACE puara_gestures::objects
  BACKENDS ${PUARA_STANDALONE_BACKENDS}
  SOURCES
Puara/PowerBandEEGAvnd.hpp
    Puara/PowerBandEEGAvnd.cpp)

avnd_addon_object(
  BASE score_addon_puara
  C_NAME puara_jab_2d_avnd
  CLASS Jab2D_Avnd
  NAMESPACE puara_gestures::objects
  BACKENDS ${PUARA_STANDALONE_BACKENDS}
  SOURCES
Puara/Jab2D_Avnd.hpp
    Puara/Jab2D_Avnd.cpp)
avnd_addon_object(
  BASE score_addon_puara
  C_NAME puara_shake
  CLASS Shake
  NAMESPACE puara_gestures::objects
  BACKENDS ${PUARA_STANDALONE_BACKENDS}
  SOURCES
Puara/Shake.hpp
    Puara/Shake.cpp)

avnd_addon_finalize(NAME score_addon_puara UUID 049a40e0-01d9-4040-9d00-21f6931c3035 VERSION 1)
