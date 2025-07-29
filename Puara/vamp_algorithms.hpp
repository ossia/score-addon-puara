#pragma once
#include <xtensor/containers/xadapt.hpp>
#include <xtensor/containers/xarray.hpp>
#include <xtensor/views/xview.hpp>

#include <iostream>
#include <vector>

#include <Eigen/Dense>

namespace puara_gestures::algorithms
{

class VampModel
{
public:
  VampModel(int lagtime, int n_dims)
      : m_lag(lagtime)
      , m_dims(n_dims)
      , m_data_count(0)
  {
  }

  void partial_fit(const xt::xarray<double>& data)
  {
    if(data.shape()[0] <= m_lag)
      return;

    size_t n_features = data.shape()[1];
    if(m_data_count == 0)
    {
      m_C00 = Eigen::MatrixXd::Zero(n_features, n_features);
      m_C0t = Eigen::MatrixXd::Zero(n_features, n_features);
      m_Ctt = Eigen::MatrixXd::Zero(n_features, n_features);
    }

    auto x_t = xt::view(data, xt::range(0, data.shape()[0] - m_lag), xt::all());
    auto x_tau = xt::view(data, xt::range(m_lag, data.shape()[0]), xt::all());

    Eigen::Map<
        const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>
        mat_t(x_t.data(), x_t.shape()[0], x_t.shape()[1]);
    Eigen::Map<
        const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>
        mat_tau(x_tau.data(), x_tau.shape()[0], x_tau.shape()[1]);

    // Center the data
    Eigen::VectorXd mean_t = mat_t.colwise().mean();
    Eigen::VectorXd mean_tau = mat_tau.colwise().mean();

    Eigen::MatrixXd centered_t = mat_t.rowwise() - mean_t.transpose();
    Eigen::MatrixXd centered_tau = mat_tau.rowwise() - mean_tau.transpose();

    m_C00 += centered_t.transpose() * centered_t;
    m_C0t += centered_t.transpose() * centered_tau;
    m_Ctt += centered_tau.transpose() * centered_tau;
    m_data_count += centered_t.rows();
  }

  bool solve()
  {
    if(m_data_count < m_dims || m_data_count < m_lag * 2)
      return false;
    Eigen::MatrixXd c00 = m_C00 / m_data_count;
    Eigen::MatrixXd c0t = m_C0t / m_data_count;
    Eigen::MatrixXd ctt = m_Ctt / m_data_count;
    double reg = 1e-6;
    c00 += reg * Eigen::MatrixXd::Identity(c00.rows(), c00.cols());
    ctt += reg * Eigen::MatrixXd::Identity(ctt.rows(), ctt.cols());

    // Solve the VAMP generalized eigenvalue problem: C0t^T * C00^-1 * C0t * v = λ * Ctt * v
    Eigen::LLT<Eigen::MatrixXd> chol_c00(c00);
    if(chol_c00.info() != Eigen::Success)
    {
      return false;
    }

    Eigen::MatrixXd c00_inv_c0t = chol_c00.solve(c0t);
    Eigen::MatrixXd A = c0t.transpose() * c00_inv_c0t;

    Eigen::GeneralizedEigenSolver<Eigen::MatrixXd> ges(A, ctt);
    if(ges.info() != Eigen::Success)
    {
      return false;
    }

    // Sort eigenvalues in descending order (largest first = slowest components)
    Eigen::VectorXd eigenvals = ges.eigenvalues().real();
    Eigen::MatrixXd eigenvecs = ges.eigenvectors().real();

    std::vector<std::pair<double, int>> eigen_pairs;
    for(int i = 0; i < eigenvals.size(); ++i)
    {
      if(std::isfinite(eigenvals(i)) && eigenvals(i) > 0)
      {
        eigen_pairs.emplace_back(eigenvals(i), i);
      }
    }

    if(eigen_pairs.empty())
      return false;

    // Sort by eigenvalue (descending)
    std::sort(eigen_pairs.begin(), eigen_pairs.end(), [](const auto& a, const auto& b) {
      return a.first > b.first;
    });

    // Take the top m_dims components
    int num_components = std::min(m_dims, (int)eigen_pairs.size());
    m_transform = Eigen::MatrixXd(eigenvecs.rows(), num_components);

    for(int i = 0; i < num_components; ++i)
    {
      m_transform.col(i) = eigenvecs.col(eigen_pairs[i].second);
    }

    m_is_fitted = true;
    return true;
  }

  xt::xarray<double> transform(const xt::xarray<double>& data)
  {
    if(!m_is_fitted || data.size() == 0)
      return xt::xarray<double>::from_shape({data.shape()[0], 0});

    Eigen::Map<
        const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>
        mat(data.data(), data.shape()[0], data.shape()[1]);

    Eigen::MatrixXd transformed = mat * m_transform;

    return xt::adapt(
        transformed.data(), transformed.size(), xt::no_ownership(),
        std::vector<size_t>{(size_t)transformed.rows(), (size_t)transformed.cols()});
  }

  bool is_fitted() const { return m_is_fitted; }
  int lag() const { return m_lag; }
  int dims() const { return m_dims; }

  void reset()
  {
    m_data_count = 0;
    m_is_fitted = false;
    m_C00.resize(0, 0);
    m_C0t.resize(0, 0);
    m_Ctt.resize(0, 0);
    m_transform.resize(0, 0);
  }

private:
  int m_lag;
  int m_dims;
  long long m_data_count;
  bool m_is_fitted{false};

  Eigen::MatrixXd m_C00;
  Eigen::MatrixXd m_C0t;
  Eigen::MatrixXd m_Ctt;
  Eigen::MatrixXd m_transform;
};
}
