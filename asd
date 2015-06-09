
#ifndef MBF_SP_HPP
#define MBF_SP_HPP

double mbf_sp(idx_t num_vertices, const redge_c &edges, vector<idx_t> &tour,
              idx_t source, idx_t destination, bool &negative_cycle,
              unsigned weight_idx = 0) {
  negative_cycle = false;
  raw_edge edge;
  idx_t num_edges = edges.size();
  vector<idx_t> prev(num_vertices, -1);
  vector<double> dist(num_vertices, std::numeric_limits<double>::max());

  dist[source] = 0;

  for (idx_t i = 0; i < (num_vertices - 1); ++i) {
    for (idx_t e = 0; e < num_edges; ++e) {
      edge = edges[e];
      if ((dist[edge.source] + edge.weights[weight_idx]) <
          dist[edge.destination]) {
        dist[edge.destination] = dist[edge.source] + edge.weights[weight_idx];
        prev[edge.destination] = edge.source;
      }
    }
  }

  idx_t cycle_start;
  for (idx_t e = 0; e < num_edges; ++e) {
    edge = edges[e];
    if ((dist[edge.source] + edge.weights[weight_idx]) <
        dist[edge.destination]) {
      tour.clear();
      cycle_start = prev[edge.source];
      for (idx_t a = 0; a < num_vertices; ++a)
        cycle_start = prev[cycle_start];
    

    // loop until we make a round
    idx_t cycle_curr = prev[cycle_start];
    while (cycle_curr != cycle_start) {
      tour.push_back(cycle_curr);
      cycle_curr = prev[cycle_start];
    }

    cout << cycle_start << "\n";
    cout << edge.source << "," << edge.destination << "\n"
         << "prev 0: " << prev[0] << "\n";

    negative_cycle = true;
    return 0;
    }
  }

  idx_t n = destination;
  double cost = dist[n];

  while (n != ((idx_t) - 1)) {
    tour.push_back(n);
    n = prev[n];
  }

  std::reverse(tour.begin(), tour.end());

  return cost;
}

#endif
