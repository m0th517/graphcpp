#include "graph.hpp"

#ifndef FORD_FULKERSON
#define FORD_FULKERSON

bool bfs(const graph_t &graph, idx_t sink, idx_t target, vector<idx_t> &parent,
         unsigned weight_idx = 0) {
  idx_t current_vertex;
  std::deque<idx_t> queue;

  std::vector<bool> visited(graph.size(), false);
  visited[sink] = true;
  queue.push_back(sink);
  parent[sink] = -1;

  while (!queue.empty()) {
    current_vertex = queue.front();
    queue.pop_front();

    edge_c edges = graph[current_vertex].edges;
    for (unsigned i = 0; i < edges.size(); ++i) {
      if (visited[edges[i].destination] || edges[i].weights[0] <= 0)
        continue;

      parent[edges[i].destination] = current_vertex;
      visited[edges[i].destination] = true;
      queue.push_back(edges[i].destination);
    }
  }

  return visited[target] == true;
}

double ford_fulkerson_max_flow(idx_t num_elements, const redge_c &edges,
                               idx_t source, idx_t sink,
                               unsigned weight_idx = 0) {
  idx_t u, v;
  double max_flow, path_flow;
  graph_t residual_graph(num_elements);
  vector<idx_t> parent(num_elements);

  // create backflow
  for (unsigned i = 0; i < edges.size(); ++i) {
    raw_edge e = edges[i];
    residual_graph[e.source].edges.push_back({e.destination, e.weights});
    residual_graph[e.destination].edges.push_back({e.source, {0}});
  }

  max_flow = 0;
  while (bfs(residual_graph, source, sink, parent)) {
    path_flow = std::numeric_limits<double>::max();

    for (v = sink; v != source; v = parent[v]) {
      u = parent[v];
      path_flow = fmin(path_flow,
                       graph_get_edge_weight(residual_graph, u, v, weight_idx));
    }

    for (v = sink; v != source; v = parent[v]) {
      u = parent[v];

      residual_graph[u]
          .edges[graph_get_edge_index(residual_graph, u, v)]
          .weights[weight_idx] -= path_flow;
      residual_graph[v]
          .edges[graph_get_edge_index(residual_graph, v, u)]
          .weights[weight_idx] += path_flow;

    }
    max_flow += path_flow;
  }

  return max_flow;
}

#endif
