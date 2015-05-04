#ifndef ALL_PATHS_HPP
#define ALL_PATHS_HPP

typedef void (*tourCallback)(vector<idx_t> &, void *);

void all_paths(const graph_t &graph, tourCallback callback,
               void *context = NULL, unsigned weight_idx = 0) {
  idx_t num_elements = graph.size();
  vector<idx_t> tour(num_elements);
  std::iota(std::begin(tour), std::end(tour), 0);

  do
    callback(tour, context);
  while (std::next_permutation(tour.begin(), tour.end()));
}

#endif
