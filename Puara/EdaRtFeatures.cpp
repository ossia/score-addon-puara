#include "EdaRtFeatures.hpp"

namespace puara_gestures::objects
{

float EdaRtFeatures::onepole_lp(float y, float x, double dt, float fc)
{
  if(fc <= 0.0f || dt <= 0.0) return x;
  const double a = std::exp(-2.0 * M_PI * static_cast<double>(fc) * dt);
  return static_cast<float>(a * y + (1.0 - a) * x);
}

void EdaRtFeatures::prepare(halp::setup info)
{
  setup        = info;
  _have_input  = false;
  _last_x      = -1e9f;
  _time        = 0.0;
  _scl         = 0.0f;
  _baseline    = 0.0f;

  _state           = State::Idle;
  _onset_base      = 0.0f;
  _onset_time      = 0.0;
  _peak_val        = 0.0f;
  _peak_time       = 0.0;
  _confirm_acc     = 0.0;
  _last_peak_t     = -1e9;
  _rec_target      = 0.0f;

  for(int i=0; i<VALLEY_WIN; ++i){ _val_p[i]=0.0f; _val_t[i]=0.0; }
  _val_i  = 0;
  _h_amp  = 0.0f;
  _h_rise = 0.0f;
  _h_dur  = 0.0f;

  outputs.scl              = 0.0f;
  outputs.scr              = 0.0f;
  outputs.scr_event        = 0;
  outputs.scr_amplitude_us = 0.0f;
  outputs.scr_rise_time_s  = 0.0f;
  outputs.scr_duration_s   = 0.0f;
}

void EdaRtFeatures::operator()(halp::tick t)
{
  const double dt = (setup.rate > 0.0)
    ? ((t.frames > 0) ? static_cast<double>(t.frames) / setup.rate : 1.0 / setup.rate)
    : 1.0 / 50.0;

  outputs.scr_event = 0;

  if(inputs.reset) { prepare(setup); return; }

  const float x = inputs.signal;

  // ── startup: wait for first nonzero sample ────────────────────────────────
  if(!_have_input)
  {
    if(x == 0.0f) { return; }
    _have_input = true;
    _scl        = x;
    _baseline   = 0.0f;
    _time       = 0.0;
  }

  // ── always write held outputs────────────
  outputs.scr_amplitude_us = _h_amp;
  outputs.scr_rise_time_s  = _h_rise;
  outputs.scr_duration_s   = _h_dur;

  // ── per new sample: update tonic, phasic, valley buffer, baseline ──────────
  const bool new_sample = (x != _last_x);
  if(new_sample)
  {
    _last_x = x;
    _time += dt;

    _scl = onepole_lp(_scl, x, dt, 0.05f);
  }
  const float phasic = x - _scl;

  outputs.scl = _scl;
  outputs.scr = phasic;

  if(new_sample)
  {
    // push to valley ring buffer
    _val_p[_val_i % VALLEY_WIN] = phasic;
    _val_t[_val_i % VALLEY_WIN] = _time;
    ++_val_i;

    // slow baseline EMA
    const float ba = static_cast<float>(1.0 - std::exp(-dt / 10.0));
    _baseline += (phasic - _baseline) * ba;
  }

  // ── detection thresholds ──────────────────────────────────────────────────
  static constexpr float MIN_AMP    = 0.04f;  // µS
  static constexpr float ONSET_TH   = 0.05f;  // above baseline to start rising
  static constexpr float DROP_TH    = 0.012f; // fall below peak to accumulate confirm
  static constexpr double CONFIRM_S = 0.30;   // seconds of falling to confirm peak
  static constexpr double MIN_DIST  = 1.0;    // refractory seconds
  static constexpr double MAX_REC   = 10.0;   // max recovery seconds

  if(!new_sample) return; 

  // re-arm when phasic settles near baseline
  if(phasic <= _baseline + 0.3f * ONSET_TH)
    _rearmed = true;

  // ── always emit held values ───────────────────────────────────────────────
  outputs.scr_amplitude_us = _h_amp;
  outputs.scr_rise_time_s  = _h_rise;
  outputs.scr_duration_s   = _h_dur;

  // ── state machine ─────────────────────────────────────────────────────────
  switch(_state)
  {
    case State::Idle:
    {
      const bool refract_ok = (_time - _last_peak_t >= MIN_DIST);
      if(_rearmed && refract_ok && (phasic - _baseline) >= ONSET_TH)
      {
        _rearmed     = false;
        _state       = State::Rising;
        // onset = valley (minimum phasic) in lookback window
        _onset_base = phasic; _onset_time = _time;
        for(int i=0; i<VALLEY_WIN; ++i)
          if(_val_p[i] < _onset_base){ _onset_base=_val_p[i]; _onset_time=_val_t[i]; }
        _peak_val    = phasic;
        _peak_time   = _time;
        _confirm_acc = 0.0;
      }
      break;
    }

    case State::Rising:
    {
      if(phasic > _peak_val)
      {
        _peak_val    = phasic;
        _peak_time   = _time;
        _confirm_acc = 0.0;
      }
      else if(phasic <= _peak_val - DROP_TH)
      {
        _confirm_acc += dt;
      }
      else
      {
        _confirm_acc = 0.0;
      }

      if(_confirm_acc >= CONFIRM_S)
      {
        // peak confirmed
        const float  amp    = _peak_val - _onset_base;
        const double rise_t = _peak_time - _onset_time;

        if(amp >= MIN_AMP)
        {
          _h_amp  = amp;
          _h_rise = std::min(static_cast<float>(rise_t), 3.0f);  

          outputs.scr_event        = 1;
          outputs.scr_amplitude_us = _h_amp;
          outputs.scr_rise_time_s  = _h_rise;

          _last_peak_t = _peak_time;
          _rec_target  = _onset_base + amp * 0.5f;  // 50% recovery
          _state       = State::Recovering;
        }
        else
        {
          _state = State::Idle;
        }
      }
      break;
    }

    case State::Recovering:
    {
      if(phasic <= _rec_target || _time - _peak_time > MAX_REC)
      {
        _h_dur                 = static_cast<float>(_time - _onset_time);  
        outputs.scr_duration_s = _h_dur;
        _state                 = State::Idle;
      }
      break;
    }
  }
}

} // namespace puara_gestures::objects