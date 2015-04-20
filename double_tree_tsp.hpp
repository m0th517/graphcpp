#include "kruskal_mst.hpp"

#ifndef DOUBLE_TREE_HPP
#define DOUBLE_TREE_HPP

double double_tree_tsp(idx_t num_elements, redge_c &edges,
                       vector<idx_t> &tsp_path, idx_t start_vertex = 0,
                       unsigned weight_idx = 0) {
  redge_c mst, dbl_tree;
  double total_cost = 0;

  std::sort(edges.begin(), edges.end(), comp_weight_1);
  kruskal_mst(num_elements, edges, mst);

  // dbl_tree.resize(mst.size() * 2);
  // for (idx_t i = 0, e = 0; i < mst.size(); ++i, e += 2) {
  // dbl_tree[e] = mst[i];
  // dbl_tree[e + 1] = mst[i];
  //}

  graph_t dbl_graph, full_graph;
  graph_init_from_edges(dbl_graph, num_elements, mst, UNDIRECTED);
  graph_init_from_edges(full_graph, num_elements, edges, UNDIRECTED);

  std::vector<idx_t> tour;
  graph_traverse_depth_first(dbl_graph, start_vertex, tour);

  //for (unsigned i = 0; i < tour.size(); ++i) {
    //std::cout << tour[i] << "\n";
  //}
  //for (unsigned i = 0; i < mst.size(); ++i)
    //std::cout << mst[i].source << " -> " << mst[i].destination << " ( "
              //<< mst[i].weights[weight_idx] << " )"
              //<< "\n";

  tsp_path.push_back(start_vertex);
  std::vector<bool> visited(tour.size(), false);
  for (idx_t i = 1; i < tour.size(); ++i) {
    if (!visited[tour[i]]) {
      total_cost +=
          graph_get_edge_weight(full_graph, tsp_path.back(), tour[i], weight_idx);
      tsp_path.push_back(tour[i]);
      visited[tour[i]] = true;
    }
  }


  total_cost += graph_get_edge_weight(full_graph, tsp_path.back(), start_vertex,
                                      weight_idx,UNDIRECTED);
  tsp_path.push_back(start_vertex);

  return total_cost;
}

#endif
