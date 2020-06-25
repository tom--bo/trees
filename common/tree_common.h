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
  unsigned long node_count{0};
  unsigned long node_merge{0};
  unsigned long node_split{0};
  void print() {
    printf("NodeCnt: %lu, Merge: %lu, Split: %lu\n", node_count, node_merge,
           node_split);
  }
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

  T *get_node() {
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
