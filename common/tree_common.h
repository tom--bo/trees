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
