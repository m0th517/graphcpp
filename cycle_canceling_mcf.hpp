#ifndef CYCLE_MCF_HPP
#define CYCLE_MCF_HPP

double get_cycle_cost(graph_t &graph, vector<idx_t> &parent, idx_t &cycle_start,
                      vector<idx_t> &cycle, unsigned cost_idx) {
  idx_t curr, counter;
  double cycle_cost;

  // run backward to make sure we are in the cycle
  for (idx_t i = 0; i < graph.size(); ++i)
    cycle_start = parent[cycle_start];

  // determine cost of cycle
  cycle.clear();
  cycle_cost = 0;
  // finished when cycle found or there is no cycle (count > num_vertices)
  cycle.push_back(parent[cycle_start]);
  for (curr = parent[cycle_start], counter = 0; counter < graph.size();
       curr = parent[curr], ++counter) {
    cycle_cost += graph_get_edge_weight(graph, parent[curr], curr, cost_idx);
    cycle.push_back(parent[curr]);
    if (cycle_start == curr)
      break;
  }

  std::reverse(cycle.begin(), cycle.end());

  if (counter >= graph.size())
    throw std::runtime_error("get_cycle_cost error. no cycle found");

  return cycle_cost;
}

// modified moore bellman ford algorithm
bool find_augmenting_cycle(graph_t &graph, unsigned cost_idx,
                           unsigned capacity_idx, vector<idx_t> &prev,
                           vector<idx_t> &cycle, double &cycle_cost) {
  raw_edge edge;
  redge_c edges;
  idx_t num_vertices = graph_to_edgelist(graph, edges);

  // remove edges with no capacity
  edges.erase(std::remove_if(edges.begin(), edges.end(),
                             [capacity_idx](raw_edge &edge) {
                return edge.weights[capacity_idx] <= 0;
              }),
              edges.end());

  idx_t num_edges = edges.size();

  for (idx_t source = 0; source < graph.size(); ++source) {
    cycle.clear();
    prev = vector<idx_t>(num_vertices, -1);
    vector<double> dist(num_vertices, std::numeric_limits<double>::max());

    dist[source] = 0;

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
      if ((dist[edge.source] + edge.weights[cost_idx]) <
          dist[edge.destination]) {
        idx_t cycle_start = edge.source;
        cycle_cost = get_cycle_cost(graph, prev, cycle_start, cycle, cost_idx);

        return true;
      }
    }
  }

  return false;
}

double cycle_canceling_mcf(const graph_t &graph, unsigned cost_idx,
                           unsigned capacity_idx, unsigned balance_idx = 0) {
  double balance, demand, supply, bflow, cycle_cost, ssource_cap;
  idx_t cycle_start;
  idx_t ssource = graph.size();
  idx_t ssink = ssource + 1;
  graph_t residual_graph, sgraph;
  redge_c sedges;
  vector<idx_t> prev, cycle;

  // graph that contains a super- source and sink
  sgraph = graph;
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
        "The Network has not enough supply to satisfy the demand.");

  // initial b-flow and residual graph
  graph_to_edgelist(sgraph, sedges);
  bflow = ford_fulkerson_max_flow(sgraph.size(), sedges, ssource, ssink,
                                  residual_graph, capacity_idx);

  if (bflow < supply)
    throw std::runtime_error(
        "The Network is to small to transport the available supply.");

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

  while (find_augmenting_cycle(residual_graph, cost_idx, capacity_idx, prev,
                               cycle, cycle_cost)) {
    // find max flow along cycle and cost
    double capacity, cost = 0;
    double max_flow = std::numeric_limits<double>::max();
    for (idx_t t = 1; t < cycle.size(); ++t) {
      idx_t from = cycle[t - 1], to = cycle[t];
      capacity = graph_get_edge_weight(residual_graph, from, to, capacity_idx);
      cost += graph_get_edge_weight(residual_graph, from, to, cost_idx);
      max_flow = std::fmin(max_flow, capacity);
    }

    // send flow around cycle
    for (idx_t t = 1; t < cycle.size(); ++t) {
      idx_t from = cycle[t - 1], to = cycle[t];
      residual_graph[from]
          .edges[graph_get_edge_index(residual_graph, from, to)]
          .weights[capacity_idx] -= max_flow;
      residual_graph[to]
          .edges[graph_get_edge_index(residual_graph, to, from)]
          .weights[capacity_idx] += max_flow;
    }
  }

  // find final cost
  redge_c edges;
  graph_to_edgelist(residual_graph, edges);
  double path_cost, capacity;
  double min_cost = 0, max_flow = 0;
  for (idx_t i = 0; i < edges.size(); ++i) {
    idx_t from = edges[i].source, to = edges[i].destination;
    path_cost = -edges[i].weights[cost_idx];
    capacity = edges[i].weights[capacity_idx];

    if (!graph_has_edge(sgraph, from, to) && capacity > 0) {
      min_cost += path_cost * capacity;
    }
  }

  return min_cost;
}

#endif
