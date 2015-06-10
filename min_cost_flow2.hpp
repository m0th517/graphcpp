#ifndef MIN_COST_FLOW_HPP
#define MIN_COST_FLOW_HPP

double get_cycle_cost(graph_t &graph, vector<idx_t> &parent, idx_t cycle_start,
                  unsigned cost_idx) {
  idx_t curr, counter;
  double cycle_cost;

  // run backward to make sure we are in the cycle
  for (idx_t i = 0; i < graph.size(); ++i)
    cycle_start = parent[cycle_start];

  // determine cost of cycle
  cycle_cost = 0;
  // loop finished when cycle found or there is no cycle ( count > num_vertices )
  for (curr = parent[cycle_start], counter = 0; counter < graph.size();
       curr = parent[curr], ++counter) {
    // cout << parent[curr] << " -> " << curr << "\n";
    cycle_cost += graph_get_edge_weight(graph, parent[curr], curr, cost_idx);
    if (cycle_start == curr)
      break;
  }

  if(counter >= graph.size())
      throw std::runtime_error("get_cycle_cost error. no cycle found");

  return cycle_cost;
}

bool find_augmenting_cycle(graph_t &graph, idx_t source, idx_t target,
                           unsigned cost_idx, unsigned capacity_idx,
                           vector<idx_t> &prev, idx_t &cycle_start,
                           double &cycle_cost) {
  raw_edge edge;
  redge_c edges;
  idx_t num_vertices = graph_to_edgelist(graph, edges);

  cout << "old: " << edges.size() << "\n";
  // remove edges with no capacity
  edges.erase(std::remove_if(edges.begin(), edges.end(),
                             [capacity_idx](raw_edge &edge) {
    return edge.weights[capacity_idx] <= 0;
  }));
  cout << "new: " << edges.size() << "\n";

  //for (idx_t i = 0; i < edges.size(); ++i) {
      //cout << edges[i].weights.size() << "\t"<<i << "\n";
      //cout << edges[i].source << " -> " << edges[i].destination << "\t\t" << std::flush 
           //<< edges[i].weights[0] << "\t" << edges[i].weights[1] << "\n";
    //}
  //cout << "-................\n";

  idx_t num_edges = edges.size();
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

  // for (idx_t e = 0; e < dist.size(); ++e) {
  // cout << e << "\t" << dist[e] << "\n";
  //}

  for (idx_t e = 0; e < num_edges; ++e) {
    edge = edges[e];
    if ((dist[edge.source] + edge.weights[cost_idx]) < dist[edge.destination]) {
      // run backward to make sure we are in the cycle
      cycle_start = edge.source;
      //for (idx_t i = 0; i < edges.size(); ++i)
        //cycle_start = prev[cycle_start];

      //// determine cost of cycle
      //cycle_cost = 0;
      //for (idx_t curr = prev[cycle_start];; curr = prev[curr]) {
        //// cout << prev[curr] << " -> " << curr << "\n";
        //cycle_cost += graph_get_edge_weight(graph, prev[curr], curr, cost_idx);
        //if (cycle_start == curr)
          //break;
      //}

      cycle_cost = get_cycle_cost(graph,prev,cycle_start,cost_idx);

      // if cost not negative, find another that is
      if(cycle_cost >= 0){
          cout << "looking for alternatives\n";
        for (idx_t i = 0; i < graph.size(); ++i) {
          if (i == source)
            continue;
          cycle_cost = get_cycle_cost(graph, prev, i, cost_idx);
          if( cycle_cost < 0 )
              break;
        }
      }

cout << cycle_cost << "\n";
//exit(1);

      return true;
    }
  }

  return false;
}

double cycle_canceling_mcf(const graph_t &graph, unsigned cost_idx,
                           unsigned capacity_idx, unsigned balance_idx = 0) {
  double balance, demand, supply, bflow, cycle_cost;
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

  double ssource_cap = sgraph[ssource].attributes[0];

  // initial b-flow and residual graph
  graph_to_edgelist(sgraph, sedges);
  bflow = ford_fulkerson_max_flow(sgraph.size(), sedges, ssource, ssink,
                                  residual_graph, capacity_idx);

  if (supply + demand < 0 || bflow < ssource_cap)
    throw std::runtime_error(
        "The Network has not enough supply to satisfy the demand.");

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

  while (find_augmenting_cycle(residual_graph, ssource, ssink, cost_idx,
                               capacity_idx, prev, cycle_start, cycle_cost)) {
    // redge_c residual_edges;
    // graph_to_edgelist(residual_graph, residual_edges);
    // for (idx_t i = 0; i < residual_edges.size(); ++i) {
    // if(residual_edges[i].weights[1]>0)
    // cout << residual_edges[i].source << " -> "
    //<< residual_edges[i].destination << "\t\t"
    //<< residual_edges[i].weights[0] << "\t"
    //<< residual_edges[i].weights[1] << "\n";
    //}

    // there is a cycle but it is not on our current path, explore other
    // startpoints
    //if (cycle_cost >= 0)
      //for (idx_t i = 0; cycle_cost > 0 || i < residual_graph.size(); ++i) {
        //if (i == ssource || i == ssink)
          //continue;
        //find_augmenting_cycle(residual_graph, i, ssink, cost_idx, capacity_idx,
                              //prev, cycle_start, cycle_cost);
        //cout << "i: " << i << "\n";
        //cout << "start: " << cycle_start << "\n";
        //cout << "next cost: " << cycle_cost << "\n";
      //}
    if (cycle_cost >= 0){
        cout << "ending because no negative cycle found, which is bad\n";
      break;
    }

    // for(idx_t i = 0; i < prev.size(); ++i)
    // cout << prev[i] << "\n";

    // run backwards until we find the cycle start again
    vector<idx_t> cycle{cycle_start};
    for (idx_t curr = prev[cycle_start]; curr != cycle_start;
         curr = prev[curr]) {
      cout << "loopy\n";
      cycle.push_back(curr);
    }
    cycle.push_back(cycle_start);
    std::reverse(cycle.begin(), cycle.end());

    cout << "cycle:\n";
    for (unsigned i = 0; i < cycle.size(); ++i) {
      cout << cycle[i] << "\n";
    }

    // find max flow along cycle and cost
    double capacity, cost = 0;
    double max_flow = std::numeric_limits<double>::max();
    for (idx_t t = 1; t < cycle.size(); ++t) {
      idx_t from = cycle[t - 1], to = cycle[t];
      capacity = graph_get_edge_weight(residual_graph, from, to, capacity_idx);
      cost += graph_get_edge_weight(residual_graph, from, to, cost_idx);
      max_flow = std::fmin(max_flow, capacity);
    }

    cout << "cost: " << cost << "\n"
         << "maxflow: " << max_flow << "\n";

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
  double min_cost = 0, max_flow = 0;
  for (idx_t i = 0; i < edges.size(); ++i) {
    idx_t from = edges[i].source, to = edges[i].destination;
    double path_cost = -edges[i].weights[cost_idx];
    double capacity = edges[i].weights[capacity_idx];

    if (!graph_has_edge(sgraph, from, to)) {
      max_flow += capacity;
      min_cost += path_cost;
    }
  }
  // cout <<"maxflow: " << flow << "\n max cost: " << cost << "\n";

  return min_cost;
}

double successive_shortest_paths_mcf(const graph_t &graph, unsigned cost_idx,
                                     unsigned capacity_idx) {

  return 0;
}

#endif
