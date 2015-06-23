#ifndef MAX_MATCHING_HPP
#define MAX_MATCHING_HPP

double max_matching(const graph_t &graph, idx_t num_group) {
  graph_t sgraph;
  redge_c sedges;
  idx_t ssource = graph.size();
  idx_t ssink = ssource + 1;

  // graph that contains a super- source and sink
  sgraph = graph;
  sgraph.push_back({ {}, { 0 } });
  sgraph.push_back({ {}, { 0 } });

  for (idx_t i = 0; i < graph.size(); ++i)
    if (i < num_group)
      sgraph[ssource].edges.push_back({ i, { 0, 1 } });
    else
      sgraph[i].edges.push_back({ ssink, { 0, 1 } });

  vector<weight_t> weights(1, 1.0);
  for (idx_t v = 0; v < sgraph.size(); ++v)
    for (idx_t e = 0; e < sgraph[v].edges.size(); ++e)
      sgraph[v].edges[e].weights = weights;

  graph_to_edgelist(sgraph, sedges);
  return ford_fulkerson_max_flow(sgraph.size(), sedges, ssource, ssink,
                                 residual_graph, 0);
}

#endif
