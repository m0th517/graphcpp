#ifndef ALL_PATHS_HPP
#define ALL_PATHS_HPP

void all_paths_tsp_r(const graph_t &graph, idx_t current_vertex, vector<bool> &visited,
                     vector<idx_t> current_tour, unsigned weight_idx = 0) {
  edge e;
  edge_c edges = graph[current_vertex].edges;

  if (current_tour.size() == graph.size()) {
    for (unsigned j = 0; j < current_tour.size(); ++j)
      cout << current_tour[j] << ",";
    cout << "\n";
    return;
  }

  for (idx_t i = 0; i < edges.size(); ++i) {
    e = edges[i];
    if (visited[e.destination])
      continue;
  visited[current_vertex] = true;
    current_tour.push_back(e.destination);

    all_paths_tsp_r(graph, e.destination, visited, current_tour, weight_idx);
  }
}

void all_paths_tsp(const graph_t &graph, vector<idx_t> &tour,
                   idx_t initial_vertex = 0, unsigned weight_idx = 0) {
  idx_t num_elements = graph.size();
  vector<bool> visited(num_elements, false);
  vector<idx_t> current_tour(1, initial_vertex);
  //visited[initial_vertex] = true;

  all_paths_tsp_r(graph, initial_vertex, visited, current_tour, weight_idx);
}

#endif
