#ifndef MIN_COST_FLOW_HPP
#define MIN_COST_FLOW_HPP

#include "ford_fulkerson.hpp"

bool check_negative_cycle(idx_t num_vertices, redge_c &edges, idx_t source, idx_t target,
                          vector<idx_t> &cycle, unsigned capacity_idx = 0,
                          unsigned cost_idx = 0) {
  cycle.clear();
  idx_t i;
  bool negative_cycle = false;

  //for (i = 0; i < num_vertices; ++i) {
    //if (i == target)
      //continue;
    negative_cycle = false;

    //redge_c dedges;
    // remove edges without capacity
    //for(idx_t i = 0; i < edges.size(); ++i){
        //if(edges[i].weights[capacity_idx] > 0)
            //dedges.push_back(edges[i]);
    //}

    double cost = mbf_sp(num_vertices, edges, cycle, source, target, negative_cycle,
                         cost_idx, capacity_idx);
    cout << "cost of negative cycle " << cost << "\n";

    if (negative_cycle)
      return true;
  //}

  return false;
}

void cycle_canceling_mcf(const graph_t &graph, unsigned cost_idx = 0,
                         unsigned capacity_idx = 1, unsigned balance_idx = 0) {
  double balance;
  idx_t super_source, super_sink;
  idx_t num_vertices = graph.size();

  // graph that contains a super- source and sink
  graph_t super_graph = graph;
  super_graph.push_back({{}, {0}});
  super_graph.push_back({{}, {0}});

  super_source = num_vertices;
  super_sink = num_vertices + 1;

  for (idx_t i = 0; i < num_vertices; ++i) {
    balance = graph[i].attributes[balance_idx];
    if (balance > 0) {
      // add to super source
      super_graph[super_source].attributes[0] += balance;
      super_graph[super_source].edges.push_back({i, {0, balance}});
    } else if (balance < 0) {
      // add to super sink
      super_graph[super_sink].attributes[0] += balance;
      super_graph[i].edges.push_back({super_sink, {0, -balance}});
    }
  }

  // cout << "source: " << super_graph[super_source].attributes[0] << "\n";
  // cout << "sink: " << super_graph[super_sink].attributes[0] << "\n";
  //if (super_graph[super_source].attributes[0] +
          //super_graph[super_sink].attributes[0] >
      //0) {
    //cout << "netzwerk zu klein\n";
  //}

  redge_c edges;
  graph_t residual_graph;
  graph_to_edgelist(super_graph, edges);
  // cout << "capacities: \n";
  // for(idx_t i = 0; i < super_graph.size(); ++i){
  // cout << i << ":\t" << super_graph[i].attributes[0] << "\n";
  //}

  // for(idx_t i = 0; i < edges.size(); ++i){
  // cout << edges[i].source << " -> " << edges[i].destination << "\t\t" <<
  // edges[i].weights[0] << "\t" << edges[i].weights[1] << "\n";
  //}
  double flow =
      ford_fulkerson_max_flow(super_graph.size(), edges, super_source,
                              super_sink, residual_graph, capacity_idx);
  cout << "initial flow: " << flow << "\n";

  // reverse cost
  for (idx_t i = 0; i < super_graph.size(); ++i) {
    idx_t num_edges = super_graph[i].edges.size();
    for (idx_t e = 0; e < num_edges; ++e) {
      edge curr = super_graph[i].edges[e];
      // cout << curr.destination << " <- " << i << "\n";
      // cout << graph_get_edge_index(residual_graph, curr.destination, i) <<
      // "\n";
      residual_graph[curr.destination]
          .edges[graph_get_edge_index(residual_graph, curr.destination, i)]
          .weights[cost_idx] = curr.weights[cost_idx] * -1;
    }
  }

  vector<idx_t> tour;
  redge_c residual_edges;
  graph_to_edgelist(residual_graph, residual_edges);
  // for (idx_t i = 0; i < residual_edges.size(); ++i) {
  // cout << residual_edges[i].source << " -> " << residual_edges[i].destination
  //<< "\t\t" << residual_edges[i].weights[0] << "\t"
  //<< residual_edges[i].weights[1] << "\n";
  //}

  vector<idx_t> cycle;
  int xx = 0;
  while (check_negative_cycle(residual_graph.size(), residual_edges, super_source, super_sink,
                              cycle, capacity_idx, cost_idx)) {
     cout << "cycle:\n";
     for (idx_t i = 0; i < cycle.size(); ++i) {
     cout << cycle[i] << "\n";
    }

    // find max flow along cycle
    double capacity;
    double max_flow = std::numeric_limits<double>::max();
    for (idx_t t = 1; t < cycle.size(); ++t) {
      idx_t from = cycle[t - 1];
      idx_t to = cycle[t];
      capacity = graph_get_edge_weight(residual_graph, from, to, capacity_idx);
      max_flow = std::fmin(max_flow, capacity);
    }
    cout << "max flow to sendc in cycle: " << max_flow << "\n";

    for (idx_t i = 0; i < residual_edges.size(); ++i) {
      cout << residual_edges[i].source << " -> "
           << residual_edges[i].destination << "\t\t"
           << residual_edges[i].weights[0] << "\t"
           << residual_edges[i].weights[1] << "\n";
    }
    cout << " \n\n\n";

    // update residual graph
    for (idx_t t = 1; t < cycle.size(); ++t) {
      idx_t from = cycle[t - 1];
      idx_t to = cycle[t];

      residual_graph[from]
          .edges[graph_get_edge_index(residual_graph, from, to)]
          .weights[capacity_idx] -= max_flow;

      residual_graph[to]
          .edges[graph_get_edge_index(residual_graph, to, from)]
          .weights[capacity_idx] += max_flow;
    }

    graph_to_edgelist(residual_graph, residual_edges);
    for (idx_t i = 0; i < residual_edges.size(); ++i) {
      cout << residual_edges[i].source << " -> "
           << residual_edges[i].destination << "\t\t"
           << residual_edges[i].weights[0] << "\t"
           << residual_edges[i].weights[1] << "\n";
    }
    if(xx==2)
    exit(1);
    xx++;
  }

  // mbf_sp(residual_graph.size(), residual_edges, tour, super_source,
  // super_sink,
  // negative_cycle, capacity_idx);
  // for(unsigned i =0; i < tour.size();++i)
  // cout << tour[i] << ",";
  // cout << "\n";

  // while(negative_cycle){
  // cout << "a negative cicyle\n";

  // mbf_sp(super_graph.size(), residual_edges, tour, super_source, super_sink,
  // negative_cycle, capacity_idx);
  //}
}

double successive_shortest_paths_mcf(const graph_t &graph, unsigned cost_idx,
                                     unsigned capacity_idx) {

  return 0;
}

#endif
