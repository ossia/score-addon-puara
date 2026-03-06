#pragma once
#include "halp_utils.hpp"
#include <halp/controls.hpp>
#include <halp/meta.hpp>

#include <algorithm>
#include <cmath>

namespace puara_gestures::objects
{

class EdaRtFeatures
{
public:
  halp_meta(name, "EDA")
  halp_meta(category, "Analysis/Biodata")
  halp_meta(c_name, "puara_eda_rt")
  halp_meta(author, "Luana Belinsky")
  halp_meta(description, "Real-time EDA decomposition and SCR detection.")
  halp_meta(uuid, "f0f6b04e-0e11-4acf-96bd-bf54b78078ca")

  struct
  {
    halp::data_port<"EDA", "Input EDA in µS.", float> signal;
    halp::toggle<"Reset">                             reset;
  } inputs;

  struct
  {
    halp::data_port<"SCL",           "Tonic.",                         float> scl;
    halp::data_port<"SCR",           "Phasic.",                        float> scr;
    halp::data_port<"SCR event",     "1 for one tick at SCR peak.",    int>   scr_event;
    halp::data_port<"SCR amplitude", "Peak amplitude µS.",             float> scr_amplitude_us;
    halp::data_port<"SCR rise time", "Rise time s.",                   float> scr_rise_time_s;
    halp::data_port<"SCR duration",  "Duration s.",                    float> scr_duration_s;
  } outputs;

  halp::setup setup;
  void prepare(halp::setup info);
  using tick = halp::tick;
  void operator()(halp::tick t);

private:
  static float onepole_lp(float y, float x, double dt, float fc);

  enum class State { Idle = 0, Rising = 1, Recovering = 2 };

  bool   _have_input  = false;
  float  _last_x      = -1e9f;
  double _time        = 0.0;
  float  _scl         = 0.0f;
  float  _baseline    = 0.0f;
  bool   _rearmed     = true;

  State  _state       = State::Idle;
  float  _onset_base  = 0.0f;
  double _onset_time  = 0.0;
  float  _peak_val    = 0.0f;
  double _peak_time   = 0.0;
  double _confirm_acc = 0.0;
  double _last_peak_t = -1e9;
  float  _rec_target  = 0.0f;

  // ring buffer: phasic + time, 0.75s lookback for valley detection
  static constexpr int VALLEY_WIN = 38;  // 0.75s at 50 Hz equivalent
  float  _val_p[VALLEY_WIN]{};
  double _val_t[VALLEY_WIN]{};
  int    _val_i            = 0;

  float  _h_amp       = 0.0f;
  float  _h_rise      = 0.0f;
  float  _h_dur       = 0.0f;
};

} // namespace puara_gestures::objects