#ifndef BRANCH_AND_BOUND_HPP
#define BRANCH_AND_BOUND_HPP

struct context_container {
  const graph_t *graph;
  unsigned weight_idx;

  double max_cost;
  vector<idx_t> best_tour;
};

bool walk_path(const graph_t &graph, const vector<idx_t> &tour, double max_cost,
               double &cost, unsigned weight_idx = 0) {
  cost = 0;
  idx_t tour_length = tour.size();

  if (tour_length == 0)
    return true;

  idx_t last_vertex = tour[0];
  for (idx_t current_vertex = 1; current_vertex < tour_length;
       ++current_vertex) {
    cost += graph_get_edge_weight(graph, last_vertex, current_vertex,
                                  weight_idx, UNDIRECTED);
    if (cost > max_cost)
      return false;
    last_vertex = current_vertex;
  }

  return true;
}

void path_eval_callback(vector<idx_t> &tour, void *data) {
  double cost;
  context_container *context = (context_container *)data;
  if (walk_path(*context->graph, tour, context->max_cost, cost,
                context->weight_idx)) {
      context->max_cost = cost;
      context->best_tour = tour;
  }
}

double branch_and_bound_tsp(const graph_t &graph, vector<idx_t> &tour,
                            unsigned weight_idx = 0) {
    context_container* context = new context_container();
    context->graph = &graph;
    context->weight_idx = weight_idx;
    context->max_cost = 0;
    all_paths(graph,path_eval_callback,context,weight_idx);
}

#endif
