#include "graph.hpp"

#ifndef UNION_HPP
#define UNION_HPP

using std::vector;
using std::cout;

struct unionf_t {
  idx_t cnt;
  vector<idx_t> id;
  vector<idx_t> sz;
};

void unionf_init(unionf_t &unionf, idx_t num_elements) {
  unionf.cnt = num_elements;
  unionf.id.resize(num_elements);
  unionf.sz.resize(num_elements);

  for (idx_t i = 0; i < num_elements; ++i) {
    unionf.id[i] = i;
    unionf.sz[i] = 1;
  }
}

// O(n)
idx_t unionf_find(unionf_t &unionf, idx_t set) {
  idx_t root = set;
  while (root != unionf.id[root])
    root = unionf.id[root];

  while (set != root) {
    idx_t newset = unionf.id[set];
    unionf.id[set] = root;
    set = newset;
  }

  return root;
}

void unionf_merge(unionf_t &unionf, idx_t x, idx_t y) {
  idx_t i = unionf_find(unionf, x);
  idx_t j = unionf_find(unionf, y);
  if (i == j)
    return;

  if (unionf.sz[i] < unionf.sz[j]) {
    unionf.id[i] = j;
    unionf.sz[j] += unionf.sz[i];
  } else {
    unionf.id[j] = i;
    unionf.sz[i] += unionf.sz[j];
  }
  --unionf.cnt;
}

bool unionf_connected(unionf_t &unionf, idx_t x, idx_t y) {
  return unionf_find(unionf, x) == unionf_find(unionf, y);
}

#endif
