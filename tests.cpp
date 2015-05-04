#include <cassert>
#include <iostream>
#include "unionf.hpp"
#include "graph.hpp"
#include "prim_mst.hpp"
#include "kruskal_mst.hpp"
#include "nearest_neighbour_tsp.hpp"
#include "double_tree_tsp.hpp"
#include "all_paths.hpp"
#include "branch_and_bound_tsp.hpp"

using std::cout;

void test_graph_and_components();
void test_union_struct();
void test_kruskal_mst();
void test_prim_mst();
void test_nearest_neighbour();
void test_nearest_double_tree_tsp();
void test_all_paths();
void test_branch_and_bound();

int main(int argc, char **argv) {
  //test_graph_and_components();
  //test_union_struct();
  //test_kruskal_mst();
  //test_prim_mst();
  //test_nearest_neighbour();
  //test_nearest_double_tree_tsp();
  //test_all_paths();
  test_branch_and_bound();

  cout << "ALL DONE.\n";
  return 0;
}

void test_branch_and_bound(){
  cout << "=== BRANCH AND BOUND ===\n";
  graph_t graph;
  idx_t num_elements =
      graph_init(graph, "graphs/small_paths.txt", 1, UNDIRECTED);

  vector<idx_t> tour{0,1,2,3};
  double cost;
  bool d = walk_path(graph,tour, 6, cost);
  cout << d << "\n" << cost << "\n";
  //branch_and_bound_tsp(graph,tour);

}

void print_path(vector<idx_t> &tour,void*){
    for (unsigned j = 0; j < tour.size(); ++j)
      cout << tour[j] << ",";
    cout << "\n";
}

void test_all_paths(){
  cout << "=== ALL PATHS ===\n";
  graph_t graph;
  idx_t num_elements =
      graph_init(graph, "graphs/small_paths.txt", 1, UNDIRECTED);

  vector<idx_t> tour;
  all_paths(graph,print_path);
}

void test_nearest_double_tree_tsp(){
  cout << "=== DOUBLE TREE TEST ===\n";

  redge_c edges;
  idx_t num_elements =
      edge_init(edges, "graphs/K_10.txt", 1, UNDIRECTED);


  vector<idx_t> tsp_path;
  double tsp_cost = double_tree_tsp(num_elements,edges,tsp_path);
  cout << "K_10.txt:\n";
  cout << "tour size: " << tsp_path.size() << "\n";
  cout << "tour cost: " << tsp_cost << "\n";
  cout << "tour: ";
  for(unsigned i = 0; i < tsp_path.size(); ++i)
      cout << tsp_path[i] << ", ";
  cout << "\n";
  assert(tsp_path.size() == num_elements+1);
}

void test_nearest_neighbour(){
  cout << "=== NEAREST NEIGHBOUR TEST ===\n";
  graph_t graph;
  idx_t num_elements =
      graph_init(graph, "graphs/K_10.txt", 1, UNDIRECTED);

  vector<idx_t> tsp_path;
  double tsp_cost = nearest_neighbour_tsp(graph, tsp_path);
  cout << "K_10.txt:\n";
  cout << "tour size: " << tsp_path.size() << "\n";
  cout << "tour cost: " << tsp_cost << "\n";
  cout << "tour: ";
  for(unsigned i = 0; i < tsp_path.size(); ++i)
      cout << tsp_path[i] << ", ";
  cout << "\n";
  assert(tsp_path.size() == num_elements+1);

  //num_elements =
      //graph_init(graph, "graphs/Wege2.txt", 1, UNDIRECTED);

  //tsp_path.clear();
  //tsp_cost = nearest_neighbour_tsp(graph,tsp_path);
  //cout << "Wege2.txt:\n";
  //cout << "tour size: " << tsp_path.size() << "\n";
  //cout << "tour cost: " << tsp_cost << "\n";
  //cout << "tour: ";
  //for(unsigned i = 0; i < tsp_path.size(); ++i)
      //cout << tsp_path[i] << ", ";
  //cout << "\n";
  //assert(tsp_path.size() == num_elements+1);
}

void test_union_struct() {
  cout << "=== UNION FIND TEST ===\n";

  unionf_t unionf;
  unionf_init(unionf, 10);

  for (unsigned i = 0; i < 10; ++i) {
    idx_t set = unionf_find(unionf, i);
    assert(i == set);
  }

  assert(false == unionf_connected(unionf, 0, 1));
  unionf_merge(unionf, 0, 1);
  assert(9 == unionf.cnt);
  assert(0 == unionf_find(unionf, 0));
  assert(0 == unionf_find(unionf, 1));
  assert(unionf_connected(unionf, 0, 1));

  unionf_merge(unionf, 0, 9);
  assert(8 == unionf.cnt);
  assert(0 == unionf_find(unionf, 9));

  unionf_merge(unionf, 0, 9);
  assert(8 == unionf.cnt);
}

void test_prim_mst() {
  cout << "=== PRIM TEST ===\n";

  graph_t graph;
  redge_c mst;
  idx_t num_elements =
      graph_init(graph, "graphs/SimpleWeighted.txt", 1, UNDIRECTED);

  double total_cost = prim_mst(graph, mst);
  assert(3 == mst.size());
  assert(2.5 == total_cost);

  cout << "\n";
  for (unsigned i = 0; i < mst.size(); ++i) {
    cout << mst[i].source << " -> " << mst[i].destination << " = "
         << mst[i].weights[0] << "\n";
  }

  cout << "\nG_100_200:"
       << "\n";
  num_elements = graph_init(graph, "graphs/G_100_200.txt", 1, UNDIRECTED);
  total_cost = prim_mst(graph, mst);
  cout << "prim mst test:" << total_cost << "\n";
  cout << "mst size:" << mst.size() << "\n";
}

void test_kruskal_mst() {
  cout << "=== KRUSKAL TEST ===\n";

  redge_c kedges, mst;
  idx_t num_elements =
      edge_init(kedges, "graphs/SimpleWeighted.txt", 1, UNDIRECTED);
  for (unsigned i = 0; i < kedges.size(); ++i) {
    cout << kedges[i].source << " -> " << kedges[i].destination << " = "
         << kedges[i].weights[0] << "\n";
  }
  std::sort(kedges.begin(), kedges.end(), comp_weight_1);

  double total_cost = kruskal_mst(num_elements, kedges, mst);
  assert(3 == mst.size());
  assert(2.5 == total_cost);

  cout << "\n";
  for (unsigned i = 0; i < mst.size(); ++i) {
    cout << mst[i].source << " -> " << mst[i].destination << " = "
         << mst[i].weights[0] << "\n";
  }
  cout << "total cost: " << total_cost << "\n";

  cout << "\nG_100_200:"
       << "\n";

  kedges.clear();
  num_elements = edge_init(kedges, "graphs/G_100_200.txt", 1, UNDIRECTED);
  std::sort(kedges.begin(), kedges.end(), comp_weight_1);
  total_cost = kruskal_mst(num_elements, kedges,mst);
  cout << "prim mst test:" << total_cost << "\n";
  cout << "mst size:" << mst.size() << "\n";
}

void test_graph_and_components() {
  cout << "=== GRAPH AND COMPONENTS TEST ===\n";

  // test search
  graph_t graph;
  graph_init(graph, "graphs/Graph2.txt", 0);
  assert(graph.size() == 15);
  assert(graph[0].edges.size() == 3);
  assert(graph[0].edges[0].destination == 6);
  assert(graph[8].edges.size() == 1);
  assert(graph[8].edges[0].destination == 14);

  std::vector<idx_t> path;
  graph_traverse_depth_first(graph, 0, path);
  assert(path.size() == 15);

  std::vector<idx_t> expected{ 0, 6, 9, 13 };
  for (unsigned i = 0; i < expected.size(); ++i)
    assert(path[i] == expected[i]);

  path.clear();
  graph_traverse_depth_first(graph, 4, path);
  assert(path.size() == 15);

  std::vector<idx_t> expected2{ 4, 7, 12 };
  for (unsigned i = 0; i < expected2.size(); ++i)
    assert(path[i] == expected2[i]);

  path.clear();
  graph_traverse_breadth_first(graph, 0, path);

  std::vector<idx_t> expected3{ 0, 6, 9, 13 };
  for (unsigned i = 0; i < expected3.size(); ++i)
    assert(path[i] == expected3[i]);

  // test components
  graph_init(graph, "graphs/Graph3.txt", 0, UNDIRECTED);
  assert(graph.size() == 1000);
  path.clear();
  unsigned num_components = graph_traverse_depth_first(graph, 0, path);
  assert(4 == num_components);
  assert(graph[41].edges[0].destination == 467);
  assert(graph[169].edges[0].destination == 724);

  graph_init(graph, "graphs/Graph4.txt", 0, UNDIRECTED);
  path.clear();
  num_components = graph_traverse_depth_first(graph, 0, path);
  assert(4 == num_components);
}
