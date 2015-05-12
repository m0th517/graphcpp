#include <cassert>
#include <fstream>
#include <deque>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include "graph.hpp"

void graph_init_from_edges(graph_t &graph, idx_t num_elements, redge_c &edges,
                           graph_direction dir) {
  raw_edge edge;
  graph = graph_t(num_elements);
  for (idx_t i = 0; i < edges.size(); ++i) {
    edge = edges[i];
    graph[edges[i].source].edges.push_back({ edge.destination, edge.weights });

    if (dir == UNDIRECTED)
      graph[edges[i].destination].edges.push_back(
          { edge.source, edge.weights });
  }
}

idx_t graph_init(graph_t &graph, std::string filename, int num_weights,
                 graph_direction dir) {
  edge e;
  idx_t source, undir_source, num_vertices;
  std::string line;
  std::vector<std::string> parts;

  std::ifstream file(filename.c_str());

  // first line is number of vertices
  std::getline(file, line);
  num_vertices = strtol(line.c_str(), NULL, 10);
  graph = graph_t(num_vertices);

  while (file) {
    std::getline(file, line);

    if (line.empty())
      continue;

    parts = str_split(line, '\t');
    assert(2 + num_weights == parts.size());

    source = strtol(parts[0].c_str(), NULL, 10);
    e.destination = strtol(parts[1].c_str(), NULL, 10);
    e.weights.resize(num_weights);

    for (unsigned i = 2; i < (num_weights + 2); ++i)
      e.weights[i - 2] = strtof(parts[i].c_str(), NULL);

    graph[source].edges.push_back(e);

    // if undirected graph, insert the other way too
    if (dir == UNDIRECTED) {
      undir_source = e.destination;
      e.destination = source;
      graph[undir_source].edges.push_back(e);
    }
  }
  return num_vertices;
}

idx_t edge_init(redge_c &edges, std::string filename, int num_weights,
                graph_direction dir) {
  raw_edge e;
  idx_t undir_source;
  size_t num_vertices;
  std::string line;
  std::vector<std::string> parts;

  std::ifstream file(filename.c_str());

  // first line is number of vertices
  std::getline(file, line);
  num_vertices = strtol(line.c_str(), NULL, 10);

  while (file) {
    std::getline(file, line);

    if (line.empty())
      continue;

    parts = str_split(line, '\t');
    assert(2 + num_weights == parts.size());

    e.source = strtol(parts[0].c_str(), NULL, 10);
    e.destination = strtol(parts[1].c_str(), NULL, 10);
    e.weights.resize(num_weights);

    for (unsigned i = 2; i < (num_weights + 2); ++i)
      e.weights[i - 2] = strtof(parts[i].c_str(), NULL);

    edges.push_back(e);

    // if undirected graph, insert the other way too
    if (dir == UNDIRECTED) {
      undir_source = e.destination;
      e.destination = e.source;
      e.source = undir_source;
      edges.push_back(e);
    }
  }
  return num_vertices;
}

unsigned graph_traverse_depth_first(const graph_t &graph, idx_t start_vertex,
                                    std::vector<idx_t> &path) {
  unsigned num_components = 1;
  std::vector<bool> visited(graph.size(), false);
  path.push_back(start_vertex);
  graph_traverse_depth_first_r(graph, start_vertex, visited, path);

  for (unsigned i = 0; i < graph.size(); ++i) {
    if (visited[i])
      continue;
    ++num_components;
    path.push_back(i);
    graph_traverse_depth_first_r(graph, i, visited, path);
  }

  return num_components;
}

void graph_traverse_depth_first_r(const graph_t &graph, idx_t current_vertex,
                                  std::vector<bool> &visited,
                                  std::vector<idx_t> &path) {
  visited[current_vertex] = true;
  edge_c edges = graph[current_vertex].edges;
  for (unsigned i = 0; i < edges.size(); ++i) {
    if (visited[edges[i].destination])
      continue;
    path.push_back(edges[i].destination);
    graph_traverse_depth_first_r(graph, edges[i].destination, visited, path);
  }
}

void graph_traverse_breadth_first(const graph_t &graph, idx_t start_vertex,
                                  std::vector<idx_t> &path) {
  idx_t current_vertex;
  std::deque<idx_t> queue;
  std::vector<bool> visited(graph.size(), false);
  visited[start_vertex] = true;

  queue.push_back(start_vertex);
  while (!queue.empty()) {
    current_vertex = queue.front();
    queue.pop_front();

    path.push_back(current_vertex);
    edge_c edges = graph[current_vertex].edges;

    for (unsigned i = 0; i < edges.size(); ++i) {
      if (visited[edges[i].destination])
        continue;
      visited[current_vertex] = true;
      queue.push_back(edges[i].destination);
    }
  }
}

bool comp_weight_1(const raw_edge &x, const raw_edge &y) {
  return x.weights[0] < y.weights[0];
}

std::vector<std::string> &str_split(const std::string &s, char delim,
                                    std::vector<std::string> &elems) {
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
  }
  return elems;
}

std::vector<std::string> str_split(const std::string &s, char delim) {
  std::vector<std::string> elems;
  str_split(s, delim, elems);
  return elems;
}

double graph_get_edge_weight(const graph_t &graph, idx_t source,
                             idx_t destination, unsigned weight_idx) {
  edge_c neighbours = graph[source].edges;
  for (idx_t i = 0; i < neighbours.size(); ++i)
    if (neighbours[i].destination == destination)
      return neighbours[i].weights[weight_idx];
  throw std::runtime_error("Edge weight not found.");
}
