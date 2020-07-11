#include <list>
#include <queue>
#include <stdio.h>
#include <unordered_map>
#include <vector>
#define UNUSED(x) ((void)x)

enum IndexType {
  B = 0,
  B_PLUS = 1,
  B_STAR = 2,
};

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
  double root_key_count{0};
  double node_count{0};
  double node_merge{0};
  double node_split{0};
  double intermediate_node_cnt{0};
  double leaf_node_cnt{0};
  double intermediate_node_keys_cnt{0};
  double leaf_node_keys_cnt{0};
  double cache_hit_cnt{0};
  double cache_checked{0};

  void print() {
    printf("NodeCnt: %.1f, Merge: %.1f, Split: %.1f\n", node_count, node_merge,
           node_split);
  }
};

class Inode {
public:
  virtual void reset() = 0;
};

/*
 * Indexable (Interface class of trees and skiplist)
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
 * INodeManager
 */
class INodeManager {
public:
  virtual void check_node(Inode *) = 0;
  virtual Inode *create_node() = 0;
  virtual void return_node(Inode *) = 0;
};

/*
 * NodeManager
 */
class NodeManager : public INodeManager {
  IndexType index_type;
  unsigned int pool_cnt;
  short t;
  std::vector<Inode *> node_pool;
  std::queue<Inode *> returned_queue;

public:
  NodeManager(short t_num, int node_cnt, IndexType it)
      : index_type{it}, t{t_num} {
    pool_cnt = 0;
    node_pool = std::vector<Inode *>(node_cnt);
  }
  void check_node(Inode *node) override {
    // DO NOTHING
    UNUSED(node);
    return;
  }
  Inode *create_node() override;
  void return_node(Inode *n) override;
};

/*
 * LRUNodeManager
 */
class LRUNodeManager : public INodeManager {
  IndexType index_type;
  unsigned int pool_cnt;
  short t;
  MetricCounter *metric_counter;
  std::vector<Inode *> node_pool;
  std::queue<Inode *> returned_queue;
  // LRU cache
  std::list<uintptr_t> queue;
  std::unordered_map<uintptr_t, std::list<uintptr_t>::iterator> itr;
  unsigned int capa;
  int disk_access_penalty_us;

public:
  LRUNodeManager(short t_num, unsigned int node_cnt, IndexType it,
                 MetricCounter *mc)
      : index_type{it}, t{t_num}, metric_counter{mc}, capa{node_cnt},
        disk_access_penalty_us{100} {
    pool_cnt = 0;
    node_pool = std::vector<Inode *>(node_cnt);
  }

  Inode *create_node() override;
  void return_node(Inode *n) override;
  void check_node(Inode *node) override;
  void cache_node(Inode *np);
  bool get_lru(Inode *np);
};
