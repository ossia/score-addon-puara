#pragma once

#include <halp/controls.hpp>
#include <halp/meta.hpp>

#include <vector>

#include <Eigen/Dense>

namespace puara_gestures::objects
{

class PCAAvnd
{
public:
  halp_meta(name, "PCA")
  halp_meta(category, "AI/Data processing")
  halp_meta(c_name, "puara_pca_avnd")
  halp_meta(description, "Performs Principal Component Analysis on a dataset.")
  halp_meta(uuid, "0a1b2c3d-4e5f-6a7b-8c9d-0e1f2a3b4c5d")

  struct ins
  {
    halp::val_port<"Data", std::vector<double>> data;
    halp::knob_i32<"Num Features", halp::range{1, 128, 2}> n_features;
    halp::knob_i32<"Num Components", halp::range{1, 10, 2}> n_components;
    halp::impulse_button<"Reset"> reset;
  } inputs;

  struct outs
  {
    halp::val_port<"Principal Components", std::vector<double>> principal_components;
  } outputs;

  void operator()();

private:
  std::vector<double> m_principal_components;
  bool m_is_computed{false};

  mutable Eigen::MatrixXd m_data_matrix;
  mutable Eigen::MatrixXd m_centered_data;
  mutable Eigen::MatrixXd m_covariance;
  mutable Eigen::MatrixXd m_components;
  mutable Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> m_eigen_solver;
  mutable int m_cached_n_samples{0};
  mutable int m_cached_n_features{0};
};

}
