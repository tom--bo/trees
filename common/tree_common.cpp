#include "bplustree.h"
#include "bstartree.h"
#include "btree.h"
#include <unistd.h>
using namespace std;

/*
 * NodeManager
 */
Inode *NodeManager::create_node() {
  Inode *n;
  if (!returned_queue.empty()) {
    n = returned_queue.front();
    returned_queue.pop();
    return n;
  }
  if (pool_cnt != node_pool.size() - 1) {
    switch (index_type) {
    case B:
      n = (Inode *)new BNode(t);
      break;
    case B_PLUS:
      n = (Inode *)new BpNode(t);
      break;
    case B_STAR:
      n = (Inode *)new BsNode(t);
      break;
    }
    node_pool.push_back(n);
    pool_cnt++;
    return n;
  }
  return node_pool[pool_cnt++];
}

void NodeManager::return_node(Inode *n) {
  n->reset();
  returned_queue.push(n);
}

/*
 * LRUNodeManager
 */
bool LRUNodeManager::get_lru(Inode *np) {
  uintptr_t p = uintptr_t(np);
  if (itr.count(p) == 0) {
    return false;
  }
  queue.erase(itr[p]);
  queue.push_front(p);
  itr[p] = queue.begin();
  return true;
}

void LRUNodeManager::cache_node(Inode *np) {
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

void LRUNodeManager::check_node(Inode *node) {
  metric_counter->cache_checked += 1;
  if (get_lru(node)) {
    metric_counter->cache_hit_cnt += 1;
    return;
  }
  usleep(disk_access_penalty_us);
}

Inode *LRUNodeManager::create_node() {
  Inode *n;
  if (!returned_queue.empty()) {
    n = returned_queue.front();
    returned_queue.pop();
  } else if (pool_cnt != node_pool.size() - 1) {
    switch (index_type) {
    case B:
      n = (Inode *)new BNode(t);
      break;
    case B_PLUS:
      n = (Inode *)new BpNode(t);
      break;
    case B_STAR:
      n = (Inode *)new BsNode(t);
      break;
    }
    node_pool.push_back(n);
    pool_cnt++;
  } else {
    n = node_pool[pool_cnt++];
  }
  cache_node(n);
  return n;
}

void LRUNodeManager::return_node(Inode *n) {
  n->reset();
  returned_queue.push(n);
}
