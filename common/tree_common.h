#include <bits/stdc++.h>
#include <unistd.h>
using namespace std;
/*
 * Item
 */
struct Item {
  unsigned long key;
  unsigned long val;
  bool operator<(const Item &right) const {
    return key == right.key ? val < right.val : key < right.key;
  }
};

/*
 * MetricCounter
 */
struct MetricCounter {
  double height{0};
  double node_count{0};
  double node_merge{0};
  double node_split{0};
  double intermediate_node_cnt{0};
  double leaf_node_cnt{0};
  double intermediate_node_keys_cnt{0};
  double leaf_node_keys_cnt{0};

  void print() {
    printf("NodeCnt: %.1f, Merge: %.1f, Split: %.1f\n", node_count, node_merge,
           node_split);
  }
};

/*
 * Indexable
 */
class Indexable {
public:
  virtual void update_metric() = 0;
  virtual unsigned get_key_max() = 0;
  virtual void insert(Item k) = 0;
  virtual bool delete_key(unsigned long k) = 0;
  virtual Item search(unsigned long k) = 0;
  virtual unsigned long count(unsigned long k) = 0;
  virtual unsigned long count_range(unsigned long min_, unsigned long max_) = 0;
  virtual void tree_walk(std::vector<Item> *v) = 0;
  virtual void print_metrics() = 0;
  virtual MetricCounter get_metrics() = 0;
};

/*
 * NodeManager
 */
template <class T> class NodeManager {
  unsigned int pool_cnt;
  short t;
  std::vector<T *> node_pool;
  std::queue<T *> returned_queue;

public:
  NodeManager(short t_num, int node_cnt) : t{t_num} {
    pool_cnt = 0;
    node_pool = std::vector<T *>(node_cnt);
  }

  void check_node(T *node) { return; }

  T *create_node() {
    T *n;
    if (!returned_queue.empty()) {
      n = returned_queue.front();
      returned_queue.pop();
      return n;
    }
    if (pool_cnt != node_pool.size() - 1) {
      n = new T(t);
      node_pool.push_back(n);
      pool_cnt++;
      return n;
    }
    return node_pool[pool_cnt++];
  }

  void return_node(T *n) {
    n->reset();
    returned_queue.push(n);
  }
};

/*
 * LRUNodeManager
 */
template <class T> class LRUNodeManager {
  unsigned int pool_cnt;
  short t;
  std::vector<T *> node_pool;
  std::queue<T *> returned_queue;
  // LRU cache
  list<uintptr_t> queue;
  unordered_map<uintptr_t, list<uintptr_t>::iterator> itr;
  unsigned int capa;
  int disk_access_penalty_us;

public:
  LRUNodeManager(short t_num, int node_cnt)
      : t{t_num}, capa{10000}, disk_access_penalty_us{100} {
    pool_cnt = 0;
    node_pool = std::vector<T *>(node_cnt);
  }

  bool get_lru(T *np) {
    uintptr_t p = uintptr_t(np);
    if (itr.count(p) == 0) {
      return false;
    }
    queue.erase(itr[p]);
    queue.push_front(p);
    itr[p] = queue.begin();
    return true;
  }

  void cache_node(T *np) {
    uintptr_t p = uintptr_t(np);
    if (itr.count(p) != 0) {
      queue.erase(itr[p]);
    } else {
      usleep(disk_access_penalty_us);
    }
    queue.push_front(p);
    itr[p] = queue.begin();
    if (queue.size() > capa) {
      auto k = *(--queue.end());
      itr.erase(k);
      queue.pop_back();
    }
  }

  void check_node(T *node) {
    if (get_lru(node)) {
      return;
    }
    usleep(disk_access_penalty_us);
  }

  T *create_node() {
    T *n;
    if (!returned_queue.empty()) {
      n = returned_queue.front();
      returned_queue.pop();
    } else if (pool_cnt != node_pool.size() - 1) {
      n = new T(t);
      node_pool.push_back(n);
      pool_cnt++;
    } else {
      n = node_pool[pool_cnt++];
    }
    cache_node(n);
    return n;
  }

  void return_node(T *n) {
    n->reset();
    returned_queue.push(n);
  }
};
