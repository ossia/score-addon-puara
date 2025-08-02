#include "PCAAvnd.hpp"

#include <Eigen/Dense>

namespace puara_gestures::objects
{

void PCAAvnd::operator()()
{
  if(!inputs.reset.value.has_value())
  {
    m_is_computed = false;
    m_principal_components.clear();
  }
  if(m_is_computed)
  {
    outputs.principal_components.value = m_principal_components;
    return;
  }
  outputs.principal_components.value.clear();

  const auto& input_vec = inputs.data.value;
  const int n_features = inputs.n_features.value;
  const int n_components = inputs.n_components.value;
  if(input_vec.empty() || n_features <= 0 || (input_vec.size() % n_features != 0))
  {
    return;
  }
  const int n_samples = input_vec.size() / n_features;
  if(n_samples < n_features)
  {
    return;
  }
  // Resizing matrices only if dimensions changed (avoiding unnecessary allocations)-- change
  if(m_cached_n_samples != n_samples || m_cached_n_features != n_features)
  {
    m_data_matrix.resize(n_samples, n_features);
    m_centered_data.resize(n_samples, n_features);
    m_covariance.resize(n_features, n_features);
    m_components.resize(n_features, n_components);
    m_cached_n_samples = n_samples;
    m_cached_n_features = n_features;
  }

  // Mapping the  input data to matrix (no copy, just viewing)
  Eigen::Map<
      const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>
      data_view(input_vec.data(), n_samples, n_features);
  m_data_matrix = data_view;
  m_centered_data = m_data_matrix.rowwise() - m_data_matrix.colwise().mean();
  m_covariance.noalias()
      = (m_centered_data.transpose() * m_centered_data) / (n_samples - 1.0);
  m_eigen_solver.compute(m_covariance);
  if(m_eigen_solver.info() != Eigen::Success)
  {
    return;
  }

  const auto& eigenvectors = m_eigen_solver.eigenvectors();
  if(eigenvectors.cols() < n_components)
  {
    return; // Not enough components found
  }

  // Extracting components and reversing (reuse m_components matrix)
  m_components = eigenvectors.rightCols(n_components).rowwise().reverse();
  m_principal_components.assign(
      m_components.data(), m_components.data() + m_components.size());
  m_is_computed = true;
  outputs.principal_components.value = m_principal_components;
}

}
