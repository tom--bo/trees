#include <bits/stdc++.h>

struct Item {
  unsigned long key;
  unsigned long val;
  bool operator<(const Item &right) const {
    return key == right.key ? val < right.val : key < right.key;
  }
};

class BpNode {
public:
  unsigned short height{0};
  BpNode *left{nullptr}, *right{nullptr};
  bool is_leaf{false};
  short key_cnt{0};
  // std::vector<Item> keys; // 2*T
  // std::vector<BpNode *> c;
  Item *keys;
  BpNode **c;
  BpNode(short t) {
    // keys = std::vector<Item>(t * 2);
    // c = std::vector<BpNode *>(t * 2 + 1);
    keys = new Item[t * 2];
    c = (BpNode **)malloc(sizeof(BpNode *) * (t * 2));
    for (int i = 0; i < t * 2 + 1; i++)
      c[i] = nullptr;
  }
};

class BplustreeNodeManager {
  unsigned int pool_cnt;
  short t;
  std::vector<BpNode *> node_pool;
  std::queue<BpNode *> returned_queue;

public:
  BplustreeNodeManager(short t, int node_cnt);
  BpNode *get_node();
  void return_node(BpNode *n);
};

struct MetricCounter {
  int node_merge{0};
  int node_split{0};
  void print() { printf("Merge: %d, Split: %d\n", node_merge, node_split); }
};

class Bplustree {
private:
  BpNode *root;
  short t;
  short key_max; // 2*T
  short key_min; // T
  MetricCounter mc;
  BplustreeNodeManager nm;

public:
  // Bplus-Tree-Create
  Bplustree(short t_num);
  void insert(Item k);
  bool delete_key(unsigned long k);
  Item search(unsigned long k) { return search(root, k); }
  unsigned long count(unsigned long k) { return count(root, k); }
  void tree_walk(std::vector<Item> *v) { tree_walk(root, v); }
  void print_metrics() { mc.print(); }

private:
  BpNode *allocate_node();
  bool insert_nonfull(BpNode *x, Item k);
  void split_child(BpNode *x, short i);
  Item search(BpNode *x, unsigned long k);
  unsigned long count(BpNode *x, unsigned long k);
  unsigned long count_once(BpNode *x, unsigned long k);
  bool delete_key(BpNode *x, unsigned long k);
  void tree_walk(BpNode *x, std::vector<Item> *v);
  BpNode *max_leaf_node_in_subtree(BpNode *x);
  BpNode *min_leaf_node_in_subtree(BpNode *x);
  Item min_item_in_subtree(BpNode *x);
  void merge(BpNode *x, short idx);
};
