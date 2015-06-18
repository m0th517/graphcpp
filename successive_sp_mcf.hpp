#ifndef SUCC_SP_MCF_HPP
#define SUCC_SP_MCF_HPP

#include "ford_fulkerson.hpp"

double successive_shortest_paths_mcf(const graph_t &graph, unsigned cost_idx,
                                     unsigned capacity_idx,
                                     unsigned balance_idx) {
  double balance, bflow, cycle_cost;
  graph_t residual_graph;
  redge_c edges, residual_edges;

  graph_to_edgelist(graph, edges);

  // determine demand and supply
  balance = 0;
  for (idx_t i = 0; i < graph.size(); ++i)
    balance += graph[i].attributes[balance_idx];

  if (balance != 0)
    throw std::runtime_error("The Networks supply and demand are unbalanced.");

  // create residual graph
  residual_graph.resize(graph.size());
  for (idx_t i = 0; i < edges.size(); ++i) {
    raw_edge e = edges[i];
    residual_graph[e.source].edges.push_back({e.destination, e.weights});
    residual_graph[e.destination].edges.push_back({e.source, e.weights});
    residual_graph[e.destination].edges.back().weights[cost_idx] =
        -e.weights[cost_idx];
    residual_graph[e.destination].edges.back().weights[capacity_idx] = 0;
  }

  // also copy over vertex attributes
  for (idx_t i = 0; i < graph.size(); ++i)
    residual_graph[i].attributes = graph[i].attributes;

  // full flow on edges with negative cost
  graph_to_edgelist(residual_graph, residual_edges);
  for (idx_t e = 0; e < residual_edges.size(); ++e) {
    raw_edge edge = residual_edges[e];
    idx_t from = edge.source, to = edge.destination;
    double capacity = edge.weights[capacity_idx];
    double cost = edge.weights[cost_idx];
    if (capacity > 0 && cost < 0) {
      residual_graph[from]
          .edges[graph_get_edge_index(residual_graph, from, to)]
          .weights[capacity_idx] -= capacity;
      residual_graph[from].attributes[balance_idx] -= capacity;

      residual_graph[to]
          .edges[graph_get_edge_index(residual_graph, to, from)]
          .weights[capacity_idx] += capacity;
      residual_graph[to].attributes[balance_idx] += capacity;
    }
  }

  // fix unbalanced sources and sinks
  idx_t local_source, local_sink;
  vector<double> pos_balances, neg_balances;
  vector<idx_t> st_path, parent;
  bool negative_cycle = false;
  double st_cost, local_balance;
  while (true) {
    pos_balances.clear();
    neg_balances.clear();

    graph_to_edgelist(residual_graph, residual_edges);
    // print_edgelist(residual_edges);

    // remove edges with no capacity
    residual_edges.erase(std::remove_if(residual_edges.begin(),
                                        residual_edges.end(),
                                        [capacity_idx](raw_edge &edge) {
                           return edge.weights[capacity_idx] <= 0;
                         }),
                         residual_edges.end());

    for (idx_t i = 0; i < graph.size(); ++i) {
      local_balance = residual_graph[i].attributes[balance_idx];
      if (local_balance < 0)
        neg_balances.push_back(i);
      else if (local_balance > 0)
        pos_balances.push_back(i);
    }

    // if balances empty, we a are done (all balances 0)
    if (pos_balances.empty() && neg_balances.empty())
      break;

    for (idx_t s = 0; s < pos_balances.size(); ++s) {
      local_source = pos_balances[s];
      for (idx_t t = 0; t < neg_balances.size(); ++t) {
        negative_cycle = false;
        local_sink = neg_balances[t];

        // using ford fulkersons breadth first search because it returns true
        // for connected vertices. stop if not connected.
        if (bfs(residual_graph, local_source, local_sink, parent, capacity_idx))
          goto FOUND_ST_PATH;
      }
    }

    // this section is reached when no path could be found.
    // if not all balances are zero the network is to small or not connected.
    for (idx_t i = 0; i < graph.size(); ++i) {
      local_balance = residual_graph[i].attributes[balance_idx];
      if (local_balance != 0)
        throw std::runtime_error("The Network is too small or not connected.");
    }
    break;

  // exit point for nested for loop above
  FOUND_ST_PATH:

    // compute shortest (cheapest) path from s to t
    st_cost = mbf_sp(residual_graph.size(), residual_edges, st_path,
                     local_source, local_sink, negative_cycle, cost_idx);

    double max_path_flow = std::numeric_limits<double>::max();
    for (idx_t t = 1; t < st_path.size(); ++t) {
      idx_t from = st_path[t - 1], to = st_path[t];
      max_path_flow =
          std::fmin(max_path_flow, graph_get_edge_weight(residual_graph, from,
                                                         to, capacity_idx));
    }

    // find the amount of flow we can send across the path
    double max_flow =
        std::fmin(residual_graph[local_source].attributes[balance_idx],
                  -residual_graph[local_sink].attributes[balance_idx]);
    max_flow = std::fmin(max_flow, max_path_flow);

    // send min flow along path
    for (idx_t t = 1; t < st_path.size(); ++t) {
      idx_t from = st_path[t - 1], to = st_path[t];
      residual_graph[from]
          .edges[graph_get_edge_index(residual_graph, from, to)]
          .weights[capacity_idx] -= max_flow;
      residual_graph[from].attributes[balance_idx] -= max_flow;

      residual_graph[to]
          .edges[graph_get_edge_index(residual_graph, to, from)]
          .weights[capacity_idx] += max_flow;
      residual_graph[to].attributes[balance_idx] += max_flow;
    }
  }

  graph_to_edgelist(residual_graph, residual_edges);
  double min_cost = 0, max_flow = 0;
  for (idx_t i = 0; i < residual_edges.size(); ++i) {
    idx_t from = residual_edges[i].source, to = residual_edges[i].destination;
    double path_cost = -residual_edges[i].weights[cost_idx];
    double capacity = residual_edges[i].weights[capacity_idx];

    if (!graph_has_edge(graph, from, to) && capacity > 0) {
      max_flow += capacity;
      min_cost += path_cost * capacity;
    }
  }

  return min_cost;
}

#endif
