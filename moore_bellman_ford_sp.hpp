#ifndef MBF_SP_HPP
#define MBF_SP_HPP

double mbf_sp(idx_t num_vertices, const redge_c &edges, vector<idx_t> &tour,
              idx_t source, idx_t destination, bool &negative_cycle,
              unsigned weight_idx = 0, unsigned capacity_idx = 0) {
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

      //cout << edge.source << "," << edge.destination << "\n"
           //<< "prev 0: " << prev[0] << "\n";
      tour.clear();
       //for(idx_t i = 0; i < dist.size(); ++i){
       //cout << "dist " << i << ": " << dist[i]<<"\n";
      //}

      cycle_start = prev[edge.source];
      for (idx_t a = 0; a < num_vertices; ++a)
        cycle_start = prev[cycle_start];
      // cout << "cycle_start: " << cycle_start << "\n";

      // loop until we make a round
      idx_t cycle_curr = prev[cycle_start];
      tour.push_back(cycle_start);
      // cout << "cycle_curr: " << cycle_start << "\n";
      // cout << "prev:\n";
      // for (idx_t a = 0; a < prev.size(); ++a)
      // cout << a << ": " << prev[a] << "\n";
      // exit(1);
      while (cycle_curr != cycle_start) {
        // cout << "cycle_curr: " << cycle_start << "\n";
        tour.push_back(cycle_curr);
        cycle_curr = prev[cycle_curr];
      }

      // cout << cycle_start << "\n";
      // cout << edge.source << "," << edge.destination << "\n"
      //<< "prev 0: " << prev[0] << "\n";

      // compute cost for cycle
      double cost = 0;
      tour.push_back(tour[0]);
      std::reverse(tour.begin(),tour.end());
      for (idx_t i = 1; i < tour.size(); ++i) {
        idx_t from = tour[i - 1];
        idx_t to = tour[i];

        for (idx_t g = 0; g < num_edges; ++g) {
          if (edges[g].source == from && edges[g].destination == to) {
              //cout <<"from:" << from << ", to: " << to <<"  ##" << edges[g].weights[weight_idx] << "\n";
              cost += edges[g].weights[weight_idx];
            break;
          }
        }
      }
      //cout << "the cost is " << cost << "\n";

      negative_cycle = true;
      return cost;
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
