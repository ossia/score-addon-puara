cmake_minimum_required(VERSION 3.24...3.30)

project(extras VERSION 1.0 LANGUAGES CXX)

# Define the library sources
add_library(extras
  PeakDetector.cpp
  PeakDetector.h
  helpers.cpp
  helpers.h
  Normalizer.cpp
  Normalizer.h
  EMA.h
  MinMaxScaler.cpp
  MinMaxScaler.h
  QuantileScaler.cpp
  QuantileScaler.h
)
