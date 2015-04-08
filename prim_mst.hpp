#include <queue>
#include "graph.hpp"
#include "unionf.hpp"

#ifndef PRIM_HPP
#define PRIM_HPP

class redge_comparator {
  unsigned weight_idx;

public:
  redge_comparator(const unsigned &weight_idx = 0) : weight_idx(weight_idx) {}

  bool operator()(const raw_edge &a, const raw_edge &b) {
    return a.weights[weight_idx] > b.weights[weight_idx];
  }
};

typedef std::priority_queue<raw_edge, vector<raw_edge>, redge_comparator>
redgepq;

double prim_mst(const graph_t &graph, redge_c &mst, unsigned weight_idx = 0) {
  double cost;
  raw_edge redge;
  edge_c new_edges;

  redgepq pqueue;
  idx_t current_vertex = 0;
  idx_t num_elements = graph.size();
  vector<bool> in_tree(num_elements, false);
  mst.clear();

  in_tree[0] = true;
  for (idx_t e = 0; e < graph[0].edges.size(); ++e) {
    pqueue.push(
        { 0, graph[0].edges[e].destination, graph[0].edges[e].weights });
  }

  cost = 0;
  while (!pqueue.empty()) {
    redge = pqueue.top();
    pqueue.pop();

    if (in_tree[redge.destination])
      continue;

    mst.push_back(redge);
    in_tree[redge.destination] = true;
    cost += redge.weights[weight_idx];

    new_edges = graph[redge.destination].edges;
    for (idx_t e = 0; e < new_edges.size(); ++e) {
      if (in_tree[new_edges[e].destination])
        continue;
      pqueue.push({ 0, new_edges[e].destination, new_edges[e].weights });
    }
  }

  return cost;
}

#endif
