#pragma once

#include <halp/controls.hpp>
#include <halp/meta.hpp>

#include <vector>

namespace puara_gestures::objects
{

class ClusteringAvnd
{
public:
  halp_meta(name, "Clustering")
  halp_meta(category, "AI/Data processing")
  halp_meta(c_name, "puara_clustering_avnd")
  halp_meta(description, "Performs KMeans or Agglomerative clustering on input data.")
  halp_meta(uuid, "8fa7709c-4232-47b8-82be-4889ae08656d")

  enum class Algorithm
  {
    KMeans,
    Agglomerative
  };

  struct ins
  {
    halp::val_port<"Matrix", std::vector<double>> matrix;
    halp::enum_t<Algorithm, "Algorithm"> algorithm{Algorithm::KMeans};
    halp::knob_i32<"Num Clusters", halp::range{1, 20, 2}> n_clusters;
    halp::knob_i32<"Num Features", halp::range{1, 128, 1}> n_features;
  } inputs;

  struct outs
  {
    halp::val_port<"Cluster Labels", std::vector<double>> cluster_labels;
    halp::val_port<"Cluster Centers", std::vector<double>>
        cluster_centers; // For KMeans only
  } outputs;

  void operator()();
};
}
