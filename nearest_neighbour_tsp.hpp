#include "graph.hpp"

#ifndef NEAREST_NEIGHBOUR_HPP
#define NEAREST_NEIGHBOUR_HPP

class edge_comparator {
  unsigned weight_idx;

public:
  edge_comparator(const unsigned &weight_idx = 0) : weight_idx(weight_idx) {}

  bool operator()(const edge &a, const edge &b) {
    return a.weights[weight_idx] < b.weights[weight_idx];
  }
};

void nearest_neighbour_tsp_r(const graph_t &graph, vector<idx_t> &tour,
                             vector<bool> &visited, double &cost,
                             idx_t current_vertex = 0,
                             unsigned weight_idx = 0) {
  edge e;
  edge_c neighbours = graph[current_vertex].edges;
  idx_t num_neighbours = neighbours.size();
  idx_t choice = num_neighbours + 1;
  std::sort(neighbours.begin(), neighbours.end(), edge_comparator(weight_idx));

  for (idx_t i = 0; i < num_neighbours; ++i) {
    e = neighbours[i];
    if (visited[e.destination])
      continue;

    choice = i;
    cost += e.weights[weight_idx];
    visited[e.destination] = true;
    tour.push_back(e.destination);
    break;
  }

  if (choice > num_neighbours)
    return;

  nearest_neighbour_tsp_r(graph, tour, visited, cost,
                          neighbours[choice].destination, weight_idx);
}

double nearest_neighbour_tsp(const graph_t &graph, vector<idx_t> &tour,
                             idx_t initial_vertex = 0,
                             unsigned weight_idx = 0) {
  double cost = 0;
  vector<bool> visited(graph.size(), false);
  visited[initial_vertex] = true;
  tour.push_back(initial_vertex);

  nearest_neighbour_tsp_r(graph, tour, visited, cost, initial_vertex,
                          weight_idx);

  // the way back
  cost += graph_get_edge_weight(graph, tour.back(), initial_vertex, weight_idx);
  tour.push_back(initial_vertex);
  return cost;
}

#endif
