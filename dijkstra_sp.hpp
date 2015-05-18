#ifndef DIJKSTRA_HPP
#define DIJKSTRA_HPP

#include <algorithm>

#define dijk_undefined ((idx_t) - 1)

template <class T> void remove_element(vector<T> &vec, idx_t index) {
  std::swap(vec[index], vec.back());
  vec.pop_back();
}

double dijkstra_sp(graph_t &graph, vector<idx_t> &tour, idx_t source,
                   idx_t destination, unsigned weight_idx = 0) {
  edge e;
  edge_c edges;
  idx_t num_visited = 0, num_vertices = graph.size(), current_vertex;
  vector<idx_t> unvisited(num_vertices);
  vector<double> dist(num_vertices, std::numeric_limits<double>::max());
  vector<idx_t> prev(num_vertices, dijk_undefined);

  std::iota(std::begin(unvisited), std::end(unvisited), 0);

  dist[source] = 0;
  prev[source] = -1;

  while (!unvisited.empty()) {
    sort(unvisited.begin(), unvisited.end(),
         [&dist](const idx_t &a, const idx_t &b)
             ->bool { return dist[a] > dist[b]; });

    current_vertex = unvisited.back();
    unvisited.pop_back();

    edges = graph[current_vertex].edges;
    for (idx_t i = 0; i < edges.size(); ++i) {
      double alt = dist[current_vertex] +
                   graph_get_edge_weight(graph, current_vertex,
                                         edges[i].destination, weight_idx);
      if (alt < dist[edges[i].destination]) {
        dist[edges[i].destination] = alt;
        prev[edges[i].destination] = current_vertex;
      }
    }

    if (current_vertex == destination)
      break;
  }

  idx_t n = destination;

  double cost = 0;
  while (n != dijk_undefined) {
    tour.push_back(n);
    if (prev[n] != dijk_undefined)
      cost += graph_get_edge_weight(graph, prev[n], n, weight_idx);
    n = prev[n];
  }
  std::reverse(tour.begin(), tour.end());

  return cost;
}

#endif
