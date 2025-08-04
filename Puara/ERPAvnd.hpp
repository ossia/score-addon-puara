#pragma once

#include <halp/audio.hpp>
#include <halp/controls.hpp>
#include <halp/meta.hpp>
#include <xtensor/containers/xarray.hpp>

#include <optional>
#include <vector>

namespace puara_gestures::objects
{

class ERPAvnd
{
public:
  halp_meta(name, "ERP")
  halp_meta(category, "Analysis/Puara")
  halp_meta(c_name, "puara_erp_avnd")
  halp_meta(
      description,
      "Computes an Event-Related Potential (ERP) from a signal and triggers.")
  halp_meta(manual_url, "https://github.com/dav0dea/goofi-pipe")
  halp_meta(uuid, "4f9c3a03-48df-4c30-ba0e-653989b37843")

  enum class BaselineCorrection
  {
    None,
    Mean
  };

  struct ins
  {
    halp::val_port<"Signal", std::vector<double>> signal;
    halp::val_port<"Trigger", std::optional<bool>> trigger;

    // --- Parameters ---
    halp::knob_f32<"Duration (s)", halp::range{0.0, 3.0, 1.0}> duration;
    halp::enum_t<BaselineCorrection, "Baseline"> baseline{BaselineCorrection::Mean};
    halp::impulse_button<"Reset"> reset;
    halp::toggle<"Delay Retrigger"> delay_retrigger{true};
  } inputs;

  struct outs
  {
    halp::val_port<"ERP", std::vector<double>> erp;
  } outputs;

  void prepare(halp::setup& setup) { m_sampling_rate = setup.rate; }
  void operator()();

private:
  void reset_state();

  // --- State Variables ---
  double m_sampling_rate{1000.0};
  bool m_is_collecting{false};
  int m_erp_count{0};
  std::vector<double> m_collected_chunk;
  xt::xarray<double> m_erp_sum;
};

}
