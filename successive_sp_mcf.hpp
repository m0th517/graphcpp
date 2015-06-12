#ifndef SUCC_SP_MCF_HPP
#define SUCC_SP_MCF_HPP

double successive_shortest_paths_mcf(const graph_t &graph, unsigned cost_idx,
                                     unsigned capacity_idx, unsigned balance_idx) {
  double balance, demand, supply, bflow, cycle_cost;
  idx_t ssource = graph.size();
  idx_t ssink = ssource + 1;
  graph_t residual_graph;
  redge_c sedges, residual_edges;

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

  // create residual graph
  residual_graph.resize(sgraph.size());
  graph_to_edgelist(sgraph, sedges);
  for (idx_t i = 0; i < sedges.size(); ++i) {
    raw_edge e = sedges[i];
    residual_graph[e.source].edges.push_back({e.destination, e.weights});
    residual_graph[e.destination].edges.push_back(
        {e.source,e.weights});
    residual_graph[e.destination].edges.back().weights[cost_idx] = -e.weights[cost_idx];
    residual_graph[e.destination].edges.back().weights[capacity_idx] = 0;
  }

  // full flow on edges with negative cost
  graph_to_edgelist(residual_graph,residual_edges);
  for(idx_t e = 0; e < residual_edges.size(); ++e){
      raw_edge edge = residual_edges[e];
      idx_t from = edge.source, to = edge.destination;
      double capacity = edge.weights[capacity_idx];
      double cost = edge.weights[cost_idx];
      if (capacity > 0 && cost < 0) {
        residual_graph[from]
            .edges[graph_get_edge_index(residual_graph, from, to)]
            .weights[capacity_idx] -= capacity;
        residual_graph[to]
            .edges[graph_get_edge_index(residual_graph, to, from)]
            .weights[capacity_idx] += capacity;
      }
  }
  graph_to_edgelist(residual_graph,residual_edges);

  return 0;
}

#endif
