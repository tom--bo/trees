#include <bits/stdc++.h>

struct Item {
  unsigned long key;
  unsigned long val;
  bool operator<(const Item &right) const {
    return key == right.key ? val < right.val : key < right.key;
  }
};

class Node {
public:
  bool is_leaf{false};
  short key_cnt{0};
  Item *keys; // 2*T-1
  Node **c;
  Node(short t) {
    keys = new Item[t * 2 - 1];
    c = (Node **)malloc(sizeof(Node *) * (t * 2));
    for (int i = 0; i < t * 2; i++)
      c[i] = nullptr;
  }
};

class BtreeNodeManager {
  unsigned int pool_cnt;
  short t;
  std::vector<Node *> node_pool;
  std::queue<Node *> returned_queue;

public:
  BtreeNodeManager(); // TODO: need to understand
  BtreeNodeManager(short t, int node_cnt);
  Node *get_node();
  void return_node(Node *n);
};

struct MetricCounter {
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
  MetricCounter mc;
  BtreeNodeManager nm;

public:
  // B-Tree-Create
  Btree(short t_num);
  void insert(Item k);
  bool delete_key(unsigned long k);
  Item search(unsigned long k) { return search(root, k); }
  void tree_walk(std::vector<Item> *v) { tree_walk(root, v); }
  void print_metrics() { mc.print(); }

private:
  Node *allocate_node();
  void insert_nonfull(Node *x, Item k);
  void split_child(Node *x, short i);
  Item search(Node *x, unsigned long k);
  bool delete_key(Node *x, unsigned long k);
  void tree_walk(Node *x, std::vector<Item> *v);
  Node *max_leaf_node_in_subtree(Node *x);
  Node *min_leaf_node_in_subtree(Node *x);
  void merge(Node *x, short idx);
};
