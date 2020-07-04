#include <bits/stdc++.h>
#ifndef treecommon
#define treecommon
#include "tree_common.h"
#endif

class BsNode : public Inode {
public:
  bool is_leaf{false};
  short key_cnt{0};
  Item *keys; // 2*T-1
  BsNode **p;
  // std::vector<Item> keys;
  // std::vector<BsNode *> p;
  BsNode(short t) {
    keys = new Item[t * 2];
    p = (BsNode **)malloc(sizeof(BsNode *) * (t * 2 + 1));
    for (int i = 0; i < t * 2; i++)
      p[i] = nullptr;
    // keys = std::vector<Item>(t * 2);
    // p = std::vector<BsNode *>(t * 2 + 1, nullptr);
  }
  void reset() override {
    key_cnt = 0;
    is_leaf = false;
  }
};

class Bstartree : public Indexable {
private:
  BsNode *root;
  short key_max; // T*2
  short key_min; // T-1
  short split_key_min;
  // split_key_min => (t_num * 2 - 1) * 2 / 3;
  MetricCounter mc;
  INodeManager *nm;

public:
  // B-Tree-Create
  Bstartree(short t_num, unsigned int l);
  void update_metric();
  unsigned get_key_max() { return key_max; };
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
  BsNode *allocate_node();
  void insert_nonfull(BsNode *x, Item k);
  void split_child(BsNode *x, short i);
  void split_root(BsNode *x);
  void move_key_to_left(BsNode *x, short i);
  void move_key_to_right(BsNode *x, short i);
  void move_keys_to_right(BsNode *x, short i, short cnt, bool to_empty);
  Item search(BsNode *x, unsigned long k);
  unsigned long count_range(BsNode *x, unsigned long min_, unsigned long max_);
  bool delete_key(BsNode *x, unsigned long k);
  void tree_walk(BsNode *x, std::vector<Item> *v);
  void tree_walk_for_metric(BsNode *x);
  BsNode *max_leaf_node_in_subtree(BsNode *x);
  BsNode *min_leaf_node_in_subtree(BsNode *x);
  void merge(BsNode *x, short idx);
};
