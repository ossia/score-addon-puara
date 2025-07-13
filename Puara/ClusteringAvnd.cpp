#include "ClusteringAvnd.hpp"

#include <algorithm>
#include <limits>
#include <numeric>
#include <random>

namespace puara_gestures::objects
{

struct KMeansResult
{
  std::vector<unsigned long> assignments;
  std::vector<std::vector<double>> centers;
};

KMeansResult custom_kmeans(
    const std::vector<std::vector<double>>& samples, int k, int max_iterations = 100)
{
  if(samples.empty() || k <= 0 || k > (int)samples.size())
  {
    return {{}, {}};
  }
  const int n_samples = samples.size();
  const int n_dims = samples[0].size();
  std::vector<std::vector<double>> centers(k);
  std::vector<int> indices(n_samples);
  std::iota(indices.begin(), indices.end(), 0);
  std::random_device rd;
  std::mt19937 gen(rd());
  std::shuffle(indices.begin(), indices.end(), gen);
  for(int i = 0; i < k; ++i)
  {
    centers[i] = samples[indices[i]];
  }
  std::vector<unsigned long> assignments(n_samples);
  for(int iter = 0; iter < max_iterations; ++iter)
  {
    bool changed = false;
    for(int i = 0; i < n_samples; ++i)
    {
      double best_dist = std::numeric_limits<double>::max();
      unsigned long best_center = 0;
      for(int j = 0; j < k; ++j)
      {
        double dist = 0.0;
        for(int d = 0; d < n_dims; ++d)
        {
          double diff = samples[i][d] - centers[j][d];
          dist += diff * diff;
        }
        if(dist < best_dist)
        {
          best_dist = dist;
          best_center = j;
        }
      }
      if(assignments[i] != best_center)
      {
        assignments[i] = best_center;
        changed = true;
      }
    }
    if(!changed)
      break;
    std::vector<std::vector<double>> new_centers(k, std::vector<double>(n_dims, 0.0));
    std::vector<int> counts(k, 0);
    for(int i = 0; i < n_samples; ++i)
    {
      int cluster_idx = assignments[i];
      for(int d = 0; d < n_dims; ++d)
      {
        new_centers[cluster_idx][d] += samples[i][d];
      }
      counts[cluster_idx]++;
    }
    for(int j = 0; j < k; ++j)
    {
      if(counts[j] > 0)
      {
        for(int d = 0; d < n_dims; ++d)
        {
          new_centers[j][d] /= counts[j];
        }
        centers[j] = new_centers[j];
      }
    }
  }
  return {assignments, centers};
}

/**
 * @brief A custom implementation of single-linkage agglomerative clustering.
 * This function starts with each point in its own cluster and iteratively
 * merges the two closest clusters until only k clusters remain.
 *
 * @author Yash Tiwari
 */
std::vector<unsigned long>
custom_agglomerative_cluster(const std::vector<std::vector<double>>& samples, int k)
{
  const int n_samples = samples.size();
  if(k <= 0 || k > n_samples)
    return {};

  std::vector<std::vector<int>> clusters(n_samples);
  for(int i = 0; i < n_samples; ++i)
  {
    clusters[i] = {i};
  }

  while(clusters.size() > (size_t)k)
  {
    double min_dist = std::numeric_limits<double>::max();
    std::pair<int, int> closest_pair = {-1, -1};

    for(size_t i = 0; i < clusters.size(); ++i)
    {
      for(size_t j = i + 1; j < clusters.size(); ++j)
      {
        for(int sample_idx_i : clusters[i])
        {
          for(int sample_idx_j : clusters[j])
          {
            double dist = 0.0;
            for(size_t d = 0; d < samples[0].size(); ++d)
            {
              double diff = samples[sample_idx_i][d] - samples[sample_idx_j][d];
              dist += diff * diff;
            }
            if(dist < min_dist)
            {
              min_dist = dist;
              closest_pair = {(int)i, (int)j};
            }
          }
        }
      }
    }

    if(closest_pair.first != -1)
    {
      int idx1 = closest_pair.first;
      int idx2 = closest_pair.second;
      clusters[idx1].insert(
          clusters[idx1].end(), clusters[idx2].begin(), clusters[idx2].end());
      clusters.erase(clusters.begin() + idx2);
    }
    else
    {
      break;
    }
  }
  std::vector<unsigned long> assignments(n_samples);
  for(size_t i = 0; i < clusters.size(); ++i)
  {
    for(int sample_idx : clusters[i])
    {
      assignments[sample_idx] = i;
    }
  }
  return assignments;
}

std::vector<std::vector<double>>
vector_to_samples(const std::vector<double>& vec, int num_cols)
{
  std::vector<std::vector<double>> samples;
  if(num_cols == 0 || vec.empty())
    return samples;
  int num_rows = vec.size() / num_cols;
  samples.reserve(num_rows);
  for(int r = 0; r < num_rows; ++r)
  {
    samples.emplace_back(vec.begin() + r * num_cols, vec.begin() + (r + 1) * num_cols);
  }
  return samples;
}

void ClusteringAvnd::operator()()
{
  const auto& input_vec = inputs.matrix.value;
  const int n_features = inputs.n_features.value;
  const int k = inputs.n_clusters.value;

  if(input_vec.empty() || n_features <= 0 || k <= 0
     || (input_vec.size() % n_features != 0))
  {
    outputs.cluster_labels.value.clear();
    outputs.cluster_centers.value.clear();
    return;
  }
  const int n_samples = input_vec.size() / n_features;
  if(n_samples < k)
  {
    outputs.cluster_labels.value.clear();
    outputs.cluster_centers.value.clear();
    return;
  }

  std::vector<unsigned long> assignments;
  auto samples = vector_to_samples(input_vec, n_features);

  if(inputs.algorithm.value == Algorithm::KMeans)
  {
    auto result = custom_kmeans(samples, k);
    assignments = result.assignments;
    std::vector<double> center_vec;
    for(const auto& center : result.centers)
    {
      center_vec.insert(center_vec.end(), center.begin(), center.end());
    }
    outputs.cluster_centers.value = std::move(center_vec);
  }
  else
  {
    assignments = custom_agglomerative_cluster(samples, k);
    outputs.cluster_centers.value.clear();
  }

  std::vector<double> labels_vec(assignments.begin(), assignments.end());
  outputs.cluster_labels.value = std::move(labels_vec);
}
}
