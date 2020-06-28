#include <bits/stdc++.h>
#ifndef treecommon
#define treecommon
#include "tree_common.h"
#endif

class BpNode {
public:
  BpNode *left{nullptr}, *right{nullptr};
  bool is_leaf{false};
  short key_cnt{0};
  // std::vector<Item> keys; // 2*T
  // std::vector<BpNode *> p;
  Item *keys;
  BpNode **p;
  BpNode(short t) {
    // keys = std::vector<Item>(t * 2);
    // p = std::vector<BpNode *>(t * 2 + 1);
    keys = new Item[t * 2];
    p = (BpNode **)malloc(sizeof(BpNode *) * (t * 2));
    for (int i = 0; i < t * 2 + 1; i++)
      p[i] = nullptr;
  }
  void reset() {
    key_cnt = 0;
    is_leaf = false;
    left = nullptr;
    right = nullptr;
  }
};

class Bplustree {
private:
  BpNode *root;
  short key_max; // 2*T
  short key_min; // T
  MetricCounter mc;
  NodeManager<BpNode> nm;

public:
  // Bplus-Tree-Create
  Bplustree(short t_num);
  void print_index_type();
  void update_metric();
  unsigned get_key_max() {
    return key_max;
  };
  void insert(Item k);
  bool delete_key(unsigned long k);
  Item search(unsigned long k) { return search(root, k); }
  unsigned long count(unsigned long k) { return count_range(root, k, k); }
  unsigned long count_range(unsigned long min_, unsigned long max_) {
    return count_range(root, min_, max_);
  }
  void tree_walk(std::vector<Item> *v) { tree_walk(root, v); }
  void print_metrics() { mc.print(); }
  MetricCounter get_metrics() { return mc; };

private:
  BpNode *allocate_node();
  bool insert_nonfull(BpNode *x, Item k);
  void split_child(BpNode *x, short i);
  Item search(BpNode *x, unsigned long k);
  unsigned long count_range(BpNode *x, unsigned long min_, unsigned long max_);
  unsigned long count_once(BpNode *x, unsigned long k);
  bool delete_key(BpNode *x, unsigned long k);
  void tree_walk(BpNode *x, std::vector<Item> *v);
  void tree_walk_for_metric(BpNode *x);
  BpNode *max_leaf_node_in_subtree(BpNode *x);
  BpNode *min_leaf_node_in_subtree(BpNode *x);
  Item min_item_in_subtree(BpNode *x);
  void merge(BpNode *x, short idx);
};
