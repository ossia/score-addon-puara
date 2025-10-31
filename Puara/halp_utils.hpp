#pragma once


#include <array>
#include <halp/audio.hpp>
#include <halp/controls.hpp>
#include <halp/meta.hpp>
#include <puara/gestures.h>
// data_port struct to include descriptions
namespace halp {
template <static_string lit, static_string desc, typename T>
struct data_port : val_port<lit, T>
{
  using base_t = val_port<lit, T>;
  using base_t::operator=;   

  static clang_buggy_consteval auto description() { return std::string_view{desc.value}; }
};

// parameter watcher to trigger when a parameter is changed
template<typename T> 
struct ParameterWatcher { 
  T last{}; 
  bool first = true; 
  bool changed(const T& current) { 
    if (first || current != last) { 
      last = current; first = false; return true; 
    } return false; 
  } 
};

} // namespace halp