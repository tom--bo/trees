#include <bits/stdc++.h>
#ifndef treecommon
#define treecommon
#include "tree_common.h"
#endif

class BNode {
public:
  bool is_leaf{false};
  short key_cnt{0};
  Item *keys; // 2*T-1
  BNode **p;
  BNode(short t) {
    keys = new Item[t * 2 - 1];
    p = (BNode **)malloc(sizeof(BNode *) * (t * 2));
    for (int i = 0; i < t * 2; i++)
      p[i] = nullptr;
  }
  void reset() {
    key_cnt = 0;
    is_leaf = false;
  }
};

struct BMetricCounter {
  int node_merge{0};
  int node_split{0};
  void print() { printf("Merge: %d, Split: %d\n", node_merge, node_split); }
};

class Btree {
private:
  BNode *root;
  short t;
  short key_max; // 2*T-1
  short key_min; // T-1
  BMetricCounter mc;
  NodeManager<BNode> nm;

public:
  // B-Tree-Create
  Btree(short t_num);
  void insert(Item k);
  bool delete_key(unsigned long k);
  Item search(unsigned long k) { return search(root, k); }
  unsigned long count(unsigned long k) { return count_range(root, k, k); }
  unsigned long count_range(unsigned long min_, unsigned long max_) {
    return count_range(root, min_, max_);
  }
  void tree_walk(std::vector<Item> *v) { tree_walk(root, v); }
  void print_metrics() { mc.print(); }

private:
  BNode *allocate_node();
  void insert_nonfull(BNode *x, Item k);
  void split_child(BNode *x, short i);
  Item search(BNode *x, unsigned long k);
  unsigned long count_range(BNode *x, unsigned long min_, unsigned long max_);
  bool delete_key(BNode *x, unsigned long k);
  void tree_walk(BNode *x, std::vector<Item> *v);
  BNode *max_leaf_node_in_subtree(BNode *x);
  BNode *min_leaf_node_in_subtree(BNode *x);
  void merge(BNode *x, short idx);
};
