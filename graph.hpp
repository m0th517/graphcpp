#include <cstdlib>
#include <vector>
#include <algorithm>
#include <string>

#ifndef GRAPH_HPP
#define GRAPH_HPP

struct edge;
struct raw_edge;
struct vertex;

typedef size_t idx_t;
typedef double attr_t;
typedef double weight_t;

typedef std::vector<edge> edge_c;
typedef std::vector<raw_edge> redge_c;
typedef std::vector<attr_t> attr_c;
typedef std::vector<weight_t> weight_c;
typedef std::vector<vertex> graph_t;

enum graph_direction { DIRECTED, UNDIRECTED };

struct raw_edge{
  idx_t source;
  idx_t destination;
  weight_c weights;
};

struct edge {
  idx_t destination;
  weight_c weights;
};

struct vertex {
  edge_c edges;
  attr_c attributes;
};

void graph_init_from_edges(graph_t &graph, idx_t num_elements, redge_c &edges);

idx_t graph_init(graph_t &graph, std::string filename, int num_weights, graph_direction dir = DIRECTED);

idx_t edge_init(redge_c &edges, std::string filename, int num_weights, graph_direction dir = DIRECTED);

unsigned graph_traverse_depth_first(const graph_t &graph, idx_t start_vertex,
                                std::vector<idx_t> &path);

void graph_traverse_depth_first_r(const graph_t &graph, idx_t current_vertex,
                                  std::vector<bool> &visited,
                                  std::vector<idx_t> &path);

void graph_traverse_breadth_first(const graph_t &graph, idx_t start_vertex,
                                  std::vector<idx_t> &path);

std::vector<std::string> &str_split(const std::string &s, char delim,
                                    std::vector<std::string> &elems);

std::vector<std::string> str_split(const std::string &s, char delim);

bool comp_weight_1(const raw_edge &x, const raw_edge &y);

#endif
