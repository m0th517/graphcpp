#ifndef BRANCH_AND_BOUND_HPP
#define BRANCH_AND_BOUND_HPP

#include <iostream>

void branch_and_bound_tsp_r(const graph_t &graph, vector<idx_t> &current_tour,
                            double current_cost, idx_t current_vertex,
                            idx_t initial_vertex, vector<bool> &visited,
                            vector<idx_t> &best_tour, double &best_cost,
                            unsigned weight_idx = 0) {
  edge e;
  double cost, back_cost;
  edge_c edges = graph[current_vertex].edges;
  idx_t num_edges = edges.size();

  if (current_tour.size() == graph.size()) {
    // the way back
    back_cost = graph_get_edge_weight(graph, current_tour.back(), initial_vertex,
                                      weight_idx);

    if ((current_cost + back_cost) > best_cost)
      return;

    best_cost = current_cost + back_cost;
    best_tour = current_tour;
    best_tour.push_back(initial_vertex);
    return;
  }

  for (idx_t i = 0; i < num_edges; ++i) {
    e = edges[i];
    if (visited[e.destination])
      continue;

    cost =
        graph_get_edge_weight(graph, current_vertex, e.destination, weight_idx);

    if ((current_cost + cost) > best_cost)
      continue;

    visited[e.destination] = true;
    //current_cost += cost;
    current_tour.push_back(e.destination);

    branch_and_bound_tsp_r(graph, current_tour, current_cost+cost, e.destination, initial_vertex,
                           visited, best_tour, best_cost, weight_idx);
    // unvisit on the way back up
    visited[e.destination] = false;
    current_tour.pop_back();
  }
}

double branch_and_bound_tsp(const graph_t &graph, vector<idx_t> &best_tour,
                            idx_t initial_vertex = 0, unsigned weight_idx = 0) {
  double best_cost = std::numeric_limits<double>::max(), current_cost = 0;
  vector<idx_t> current_tour;
  idx_t num_vertices = graph.size();
  vector<bool> visited(num_vertices, false);
  current_tour.push_back(initial_vertex);
  visited[initial_vertex] = true;

  branch_and_bound_tsp_r(graph, current_tour, current_cost, initial_vertex, initial_vertex,
                         visited, best_tour, best_cost, weight_idx);

  return best_cost;
}

#endif
