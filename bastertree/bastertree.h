#include <bits/stdc++.h>
#ifndef treecommon
#define treecommon
#include "tree_common.h"
#endif

class BaNode {
public:
  unsigned short height{0};
  BaNode *left{nullptr}, *right{nullptr};
  bool is_leaf{false};
  short key_cnt{0};
  // std::vector<Item> keys; // 2*T
  // std::vector<BaNode *> p;
  Item *keys;
  BaNode **p;
  BaNode(short t) {
    // keys = std::vector<Item>(t * 2);
    // p = std::vector<BaNode *>(t * 2 + 1);
    keys = new Item[t * 2];
    p = (BaNode **)malloc(sizeof(BaNode *) * (t * 2));
    for (int i = 0; i < t * 2 + 1; i++)
      p[i] = nullptr;
  }
};

class BastertreeNodeManager {
  unsigned int pool_cnt;
  short t;
  std::vector<BaNode *> node_pool;
  std::queue<BaNode *> returned_queue;

public:
  BastertreeNodeManager(short t, int node_cnt);
  BaNode *get_node();
  void return_node(BaNode *n);
};

struct BaMetricCounter {
  int node_merge{0};
  int node_split{0};
  void print() { printf("Merge: %d, Split: %d\n", node_merge, node_split); }
};

class Bastertree {
private:
  BaNode *root;
  short t;
  short key_max; // T*2
  short key_min; // T*2/3
  BaMetricCounter mc;
  BastertreeNodeManager nm;

public:
  // Baster-Tree-Create
  Bastertree(short t_num);
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
  BaNode *allocate_node();
  bool insert_nonfull(BaNode *x, Item k);
  void split_root(BaNode *x, short i);
  void split_child(BaNode *x, short i);
  Item search(BaNode *x, unsigned long k);
  unsigned long count_range(BaNode *x, unsigned long min_, unsigned long max_);
  unsigned long count_once(BaNode *x, unsigned long k);
  bool delete_key(BaNode *x, unsigned long k);
  void tree_walk(BaNode *x, std::vector<Item> *v);
  BaNode *max_leaf_node_in_subtree(BaNode *x);
  BaNode *min_leaf_node_in_subtree(BaNode *x);
  Item min_item_in_subtree(BaNode *x);
  void merge(BaNode *x, short idx);
};
