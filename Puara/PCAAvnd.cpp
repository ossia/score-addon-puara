#include "PCAAvnd.hpp"
#include <Eigen/Dense>

namespace puara_gestures::objects
{

void PCAAvnd::operator()()
{
  if (!inputs.reset.value.has_value()) {
    m_is_computed = false;
    m_principal_components.clear();
  }
  if (m_is_computed) {
    outputs.principal_components.value = m_principal_components;
    return;
  }
  outputs.principal_components.value.clear();

  const auto& input_vec = inputs.data.value;
  const int n_features = inputs.n_features.value;
  const int n_components = inputs.n_components.value;
  if (input_vec.empty() || n_features <= 0 || (input_vec.size() % n_features != 0)) {
    return;
  }
  const int n_samples = input_vec.size() / n_features;
  if (n_samples < n_features) {
    return;
  }
  using MatrixType = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
  Eigen::Map<const MatrixType> data_mat(input_vec.data(), n_samples, n_features);
  Eigen::MatrixXd centered_data = data_mat.rowwise() - data_mat.colwise().mean();
  Eigen::MatrixXd cov = (centered_data.transpose() * centered_data) / (n_samples - 1.0);
  Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> solver(cov);
  if (solver.info() != Eigen::Success) {
    return;
  }
  Eigen::MatrixXd eigenvectors = solver.eigenvectors();

  if (eigenvectors.cols() < n_components) {
    return; // Not enough components found
  }
  Eigen::MatrixXd components = eigenvectors.rightCols(n_components).rowwise().reverse();
  m_principal_components.assign(components.data(), components.data() + components.size());
  m_is_computed = true;
  outputs.principal_components.value = m_principal_components;
}
}
