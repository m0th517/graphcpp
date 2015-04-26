#include "kruskal_mst.hpp"

#ifndef DOUBLE_TREE_HPP
#define DOUBLE_TREE_HPP

void double_tree_tsp(idx_t num_elements, redge_c &edges,
                       vector<idx_t> &tsp_path, unsigned weight_idx = 0) {
  redge_c mst;
  double total_cost;

  std::sort(edges.begin(), edges.end(), comp_weight_1);
  total_cost = kruskal_mst(num_elements, edges, mst);
}

#endif
