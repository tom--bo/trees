#include <bits/stdc++.h>
#ifndef treecommon
#define treecommon
#include "tree_common.h"
#endif

class BsNode {
public:
  bool is_leaf{false};
  short key_cnt{0};
  Item *keys; // 2*T-1
  BsNode **p;
  BsNode(short t) {
    keys = new Item[t * 2 - 1];
    p = (BsNode **)malloc(sizeof(BsNode *) * (t * 2));
    for (int i = 0; i < t * 2; i++)
      p[i] = nullptr;
  }
};

class BsNodeManager {
  unsigned int pool_cnt;
  short t;
  std::vector<BsNode *> node_pool;
  std::queue<BsNode *> returned_queue;

public:
  BsNodeManager(short t, int node_cnt);
  BsNode *get_node();
  void return_node(BsNode *n);
};

struct BsMetricCounter {
  int node_merge{0};
  int node_split{0};
  void print() { printf("Merge: %d, Split: %d\n", node_merge, node_split); }
};

class Bstartree {
private:
  BsNode *root;
  short t;
  short key_max; // 2*T-1
  short key_min; // T-1
  BsMetricCounter mc;
  BsNodeManager nm;

public:
  // B-Tree-Create
  Bstartree(short t_num);
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
  BsNode *allocate_node();
  void insert_nonfull(BsNode *x, Item k);
  void split_child(BsNode *x, short i);
  Item search(BsNode *x, unsigned long k);
  unsigned long count_range(BsNode *x, unsigned long min_, unsigned long max_);
  bool delete_key(BsNode *x, unsigned long k);
  void tree_walk(BsNode *x, std::vector<Item> *v);
  BsNode *max_leaf_node_in_subtree(BsNode *x);
  BsNode *min_leaf_node_in_subtree(BsNode *x);
  void merge(BsNode *x, short idx);
};
