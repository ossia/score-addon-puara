#pragma once

#include <xtensor/containers/xadapt.hpp>
#include <xtensor/containers/xarray.hpp>
#include <xtensor/views/xview.hpp>
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
    }

    auto x_t = xt::view(data, xt::range(0, data.shape()[0] - m_lag), xt::all());
    auto x_tau = xt::view(data, xt::range(m_lag, data.shape()[0]), xt::all());
    Eigen::Map<
        const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>
        mat_t(x_t.data(), x_t.shape()[0], x_t.shape()[1]);
    Eigen::Map<
        const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>
        mat_tau(x_tau.data(), x_tau.shape()[0], x_tau.shape()[1]);

    m_C00 += mat_t.transpose() * mat_t;
    m_C0t += mat_t.transpose() * mat_tau;
    m_data_count += mat_t.rows();
  }

  bool solve()
  {
    if(m_data_count < m_dims)
      return false;
    Eigen::MatrixXd c00 = m_C00 / m_data_count;
    Eigen::MatrixXd c0t = m_C0t / m_data_count;
    Eigen::GeneralizedEigenSolver<Eigen::MatrixXd> ges(c0t, c00);
    m_transform = ges.eigenvectors().real();
    if(m_transform.cols() < m_dims)
      return false;
    m_transform = m_transform.leftCols(m_dims);

    m_is_fitted = true;
    return true;
  }

  xt::xarray<double> transform(const xt::xarray<double>& data)
  {
    if(!m_is_fitted)
      return {};

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
    m_transform.resize(0, 0);
  }

private:
  int m_lag;
  int m_dims;
  long long m_data_count;
  bool m_is_fitted{false};

  Eigen::MatrixXd m_C00;
  Eigen::MatrixXd m_C0t;
  Eigen::MatrixXd m_transform;
};

}
