#ifndef MIN_COST_FLOW_HPP
#define MIN_COST_FLOW_HPP

bool find_augmenting_cycle(graph_t &graph, idx_t source, idx_t target,
                           unsigned cost_idx, unsigned capacity_idx,
                           vector<idx_t> &prev, idx_t &cycle_start) {
  raw_edge edge;
  redge_c edges;
  idx_t num_vertices = graph_to_edgelist(graph, edges);

  idx_t num_edges = edges.size();
  prev = vector<idx_t>(num_vertices, -1);
  vector<double> dist(num_vertices, std::numeric_limits<double>::max());

  dist[source] = 0;

  // eig  i=1
  for (idx_t i = 0; i < (num_vertices - 1); ++i) {
    for (idx_t e = 0; e < num_edges; ++e) {
      edge = edges[e];

      if ((dist[edge.source] + edge.weights[cost_idx]) <
          dist[edge.destination]) {
        dist[edge.destination] = dist[edge.source] + edge.weights[cost_idx];
        prev[edge.destination] = edge.source;
      }
    }
  }

  for (idx_t e = 0; e < num_edges; ++e) {
    edge = edges[e];
    if ((dist[edge.source] + edge.weights[cost_idx]) < dist[edge.destination]) {
      cycle_start = edge.source;
      return true;
    }
  }

  return false;
}

double cycle_canceling_mcf(const graph_t &graph, unsigned cost_idx,
                           unsigned capacity_idx, unsigned balance_idx = 0) {
  double balance, demand, supply, bflow;
  idx_t cycle_start;
  idx_t ssource = graph.size();
  idx_t ssink = ssource + 1;
  graph_t residual_graph;
  redge_c sedges;
  vector<idx_t> prev;

  // graph that contains a super- source and sink
  graph_t sgraph = graph;
  sgraph.push_back({{}, {0}});
  sgraph.push_back({{}, {0}});

  // determine demand and supply
  demand = supply = 0;
  for (idx_t i = 0; i < graph.size(); ++i) {
    balance = graph[i].attributes[balance_idx];
    if (balance > 0) {
      supply += balance;
      sgraph[ssource].attributes[0] += balance;
      sgraph[ssource].edges.push_back({i, {0, balance}});
    } else if (balance < 0) {
      demand += balance;
      sgraph[ssink].attributes[0] += balance;
      sgraph[i].edges.push_back({ssink, {0, -balance}});
    }
  }

  if (supply + demand < 0)
    throw std::runtime_error(
        "The Network has not supply to satisfy the demand.");

  // initial b-flow and residual graph
  graph_to_edgelist(sgraph, sedges);
  bflow = ford_fulkerson_max_flow(sgraph.size(), sedges, ssource, ssink,
                                  residual_graph, capacity_idx);

  // reverse cost of residual edges
  for (idx_t i = 0; i < sgraph.size(); ++i) {
    idx_t num_edges = sgraph[i].edges.size();
    for (idx_t e = 0; e < num_edges; ++e) {
      edge curr = sgraph[i].edges[e];
      idx_t eindex = graph_get_edge_index(residual_graph, curr.destination, i);
      residual_graph[curr.destination].edges[eindex].weights[cost_idx] =
          curr.weights[cost_idx] * -1;
    }
  }

  while (find_augmenting_cycle(residual_graph, ssource, ssource, cost_idx,
                               capacity_idx, prev, cycle_start)) {

    // run backwards until we find the cycle start again
    vector<idx_t> cycle{cycle_start};
    for (idx_t curr = prev[cycle_start]; curr != cycle_start; curr = prev[curr])
      cycle.push_back(curr);
    cycle.push_back(cycle_start);
    std::reverse(cycle.begin(), cycle.end());

    // find max flow along cycle
    double capacity;
    double max_flow = std::numeric_limits<double>::max();
    for (idx_t t = 1; t < cycle.size(); ++t) {
      idx_t from = cycle[t - 1];
      idx_t to = cycle[t];
      capacity = graph_get_edge_weight(residual_graph, from, to, capacity_idx);
      max_flow = std::fmin(max_flow, capacity);
    }

    for (int i = 0; i < cycle.size(); ++i)
      cout << cycle[i] << "\n";
    exit(1);
    // cout << "prev:\n";
    // for (idx_t a = 0; a < prev.size(); ++a)
    // cout << a << ": " << prev[a] << "\n";
    // exit(1);
  }

  return 0;
}

double successive_shortest_paths_mcf(const graph_t &graph, unsigned cost_idx,
                                     unsigned capacity_idx) {

  return 0;
}

#endif
