#include <bits/stdc++.h>
#ifndef treecommon
#define treecommon
#include "tree_common.h"
#endif

class Node {
public:
  bool is_leaf{false};
  short key_cnt{0};
  Item *keys; // 2*T-1
  Node **p;
  Node(short t) {
    keys = new Item[t * 2 - 1];
    p = (Node **)malloc(sizeof(Node *) * (t * 2));
    for (int i = 0; i < t * 2; i++)
      p[i] = nullptr;
  }
};

class BtreeNodeManager {
  unsigned int pool_cnt;
  short t;
  std::vector<Node *> node_pool;
  std::queue<Node *> returned_queue;

public:
  BtreeNodeManager(short t, int node_cnt);
  Node *get_node();
  void return_node(Node *n);
};

struct BMetricCounter {
  int node_merge{0};
  int node_split{0};
  void print() { printf("Merge: %d, Split: %d\n", node_merge, node_split); }
};

class Btree {
private:
  Node *root;
  short t;
  short key_max; // 2*T-1
  short key_min; // T-1
  BMetricCounter mc;
  BtreeNodeManager nm;

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
  Node *allocate_node();
  void insert_nonfull(Node *x, Item k);
  void split_child(Node *x, short i);
  Item search(Node *x, unsigned long k);
  unsigned long count_range(Node *x, unsigned long min_, unsigned long max_);
  bool delete_key(Node *x, unsigned long k);
  void tree_walk(Node *x, std::vector<Item> *v);
  Node *max_leaf_node_in_subtree(Node *x);
  Node *min_leaf_node_in_subtree(Node *x);
  void merge(Node *x, short idx);
};
