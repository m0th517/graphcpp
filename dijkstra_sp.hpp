#ifndef DIJKSTRA_HPP
#define DIJKSTRA_HPP

#include <algorithm>

template <class T> void remove_element(vector<T> &vec, idx_t index) {
  std::swap(vec[index], vec.back());
  vec.pop_back();
}

double dijkstra_sp(graph_t &graph, vector<idx_t> &tour, idx_t source,
                   idx_t destination, unsigned weight_idx = 0) {
  edge e;
  edge_c edges;
  double min_dist;
  vector<double>::iterator min_it;
  idx_t num_visited = 0, num_vertices = graph.size(), unv_idx, min_idx,
        current_vertex;
  vector<idx_t> unvisited(num_vertices);
  vector<double> dist(num_vertices, std::numeric_limits<double>::max()),
      unv_dist;
  vector<idx_t> prev(num_vertices, -1);

  std::iota(std::begin(unvisited), std::end(unvisited), 0);

  dist[source] = 0;
  prev[source] = -1;

  while (!unvisited.empty()) {
    //unv_dist.clear();
    //unv_dist.resize(unvisited.size());
    //for (idx_t i = 0; i < unvisited.size(); ++i)
      //unv_dist[i] = dist[unvisited[i]];

    //min_it = std::min_element(unv_dist.begin(), unv_dist.end());
    //min_dist = *min_it;
    //min_idx = min_it - unv_dist.begin();

    //std::cout << "uv:\t";
    //for(unsigned i = 0; i < unvisited.size(); ++i)
        //std::cout << unvisited[i] <<","; 
    //std::cout << "\n";
    //std::cout << "di:\t";
    //for(unsigned i = 0; i < unvisited.size(); ++i)
        //std::cout << dist[unvisited[i]] <<","; 
    //std::cout << "\n";

    sort(unvisited.begin(), unvisited.end(),
         [&dist](const idx_t &a, const idx_t &b)
             ->bool { return dist[a] > dist[b]; });

    //std::cout << min_idx << "\n";
    //std::cout << "nuv:\t";
    //for(unsigned i = 0; i < unvisited.size(); ++i)
        //std::cout << unvisited[i] <<","; 
    //std::cout << "\n";

    current_vertex = unvisited.back();
    unvisited.pop_back();
    remove_element(unvisited, min_idx);

    edges = graph[current_vertex].edges;
    for (idx_t i = 0; i < edges.size(); ++i) {
      double alt = dist[current_vertex] +
                   graph_get_edge_weight(graph, current_vertex,
                                         edges[i].destination, weight_idx);
      if (alt < dist[edges[i].destination]) {
        dist[edges[i].destination] = alt;
        prev[edges[i].destination] = current_vertex;
      }
    }
  }

  idx_t n = destination;
  double cost = dist[n];

  while (n != ((idx_t) - 1)) {
    tour.push_back(n);
    n = prev[n];
  }

  std::reverse(tour.begin(), tour.end());

  return cost;
}

#endif
