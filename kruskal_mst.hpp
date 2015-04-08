#include "graph.hpp"
#include "unionf.hpp"

#ifndef KRUSKAL_HPP
#define KRUSKAL_HPP

// expects a vector of raw edges sorted according to weight_idx
double kruskal_mst(idx_t num_elements, const redge_c &edges, redge_c &mst,
                   unsigned weight_idx = 0) {
  double cost;
  unionf_t uf;
  raw_edge edge;

  unionf_init(uf, num_elements);
  mst.clear();

  for (unsigned e = 0; e < edges.size(); ++e) {
    edge = edges[e];
    if (!unionf_connected(uf, edge.source, edge.destination)) {
      unionf_merge(uf, edge.source, edge.destination);
      mst.push_back(edge);
      cost += edge.weights[weight_idx];
    }
  }

  return cost;
}

#endif
