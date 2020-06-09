#include "btree.h"
#include <bits/stdc++.h>
using namespace std;

/*
 * BtreeNodeManager
 */

BtreeNodeManager::BtreeNodeManager(short t_num, int node_cnt) : t{t_num} {
  pool_cnt = 0;
  node_pool = std::vector<Node *>(node_cnt);
}

Node *BtreeNodeManager::get_node() {
  Node *n;
  if (!returned_queue.empty()) {
    n = returned_queue.front();
    returned_queue.pop();
    return n;
  }
  if (pool_cnt != node_pool.size() - 1) {
    n = new Node(t);
    node_pool.push_back(n);
    pool_cnt++;
    return n;
  }
  return node_pool[pool_cnt++];
}

void BtreeNodeManager::return_node(Node *n) {
  n->key_cnt = 0;
  n->is_leaf = false;
  returned_queue.push(n);
}

/*
 * Btree
 */

short find_key_or_right_bound_in_node(Node *x, unsigned long k) {
  short l = -1, r = x->key_cnt, m;
  while (r - l > 1) {
    m = (l + r) / 2;
    if (x->keys[m].key >= k) {
      r = m;
    } else {
      l = m;
    }
  }
  return r;
}

Btree::Btree(short t_num) : t{t_num}, nm{BtreeNodeManager(t_num, 3000)} {
  key_max = 2 * t_num - 1;
  key_min = t_num - 1;
  nm = BtreeNodeManager(t_num, 3000);
  mc = MetricCounter();

  Node *n = allocate_node();
  n->is_leaf = true;
  root = n;
}

// Allocate-Node
Node *Btree::allocate_node() { return nm.get_node(); }

// insert
void Btree::insert(Item k) {
  Node *r = root;
  if (r->key_cnt == key_max) {
    Node *s = allocate_node();
    root = s;
    s->p[0] = r;
    split_child(s, 0);
  }
  insert_nonfull(root, k);
}

void Btree::insert_nonfull(Node *x, Item k) {
  short i = x->key_cnt;
  if (x->is_leaf) {
    while (i >= 1 && k.key < x->keys[i - 1].key) {
      x->keys[i] = x->keys[i - 1];
      i--;
    }
    x->keys[i] = k;
    x->key_cnt++;
  } else {
    i = find_key_or_right_bound_in_node(x, k.key);

    // if child node is full
    if (x->p[i]->key_cnt == key_max) {
      split_child(x, i);
      if (k.key > x->keys[i].key) {
        i++;
      }
    }
    insert_nonfull(x->p[i], k);
  }
}

void Btree::split_child(Node *x, short i) {
  mc.node_split++;
  Node *z = allocate_node();
  Node *y = x->p[i];
  z->is_leaf = y->is_leaf;

  // move second half nodes in y
  for (unsigned short j = 0; j < key_min; j++) {
    z->keys[j] = y->keys[j + t];
  }
  z->key_cnt = key_min;
  // move second half pinter if y is not leaf-node
  if (!y->is_leaf) {
    for (unsigned short j = 0; j < t; j++) {
      z->p[j] = y->p[j + t];
      y->p[j + t] = nullptr;
    }
  }

  // make key space to add center-key of y
  for (unsigned short j = x->key_cnt; j > i; j--) {
    x->keys[j] = x->keys[j - 1];
    x->p[j + 1] = x->p[j];
  }
  // move up center-key of y
  x->p[i + 1] = z;
  x->keys[i] = y->keys[t - 1];
  y->key_cnt = key_min;
  x->key_cnt++;
}

Item Btree::search(Node *x, unsigned long k) {
  short i = 0;
  // search key range or key itself
  int l = -1, m, r = x->key_cnt;
  while (r - l > 1) {
    m = (l + r) / 2;
    if (k <= x->keys[m].key) {
      r = m;
    } else {
      l = m;
    }
  }
  i = r;

  if (i <= x->key_cnt && k == x->keys[i].key) {
    return x->keys[i];
  } else if (x->is_leaf || !x->p[i]) {
    return Item{0, 0}; // return (key: 0, val: 0)
  }
  return search(x->p[i], k);
}

unsigned long Btree::count(Node *x, unsigned long k) {
  unsigned long cnt = 0;
  if (x->is_leaf) {
    for (short i = 0; i < x->key_cnt; i++) {
      if (x->keys[i].key == k)
        cnt++;
    }
    return cnt;
  }
  short i = 0;
  // search key range or key itself
  i = find_key_or_right_bound_in_node(x, k);
  cnt += count(x->p[i], k);
  while (i < x->key_cnt && x->keys[i].key == k) {
    i++;
    cnt++;
    cnt += count(x->p[i], k);
  }
  return cnt;
}

Node *Btree::min_leaf_node_in_subtree(Node *x) {
  if (x->is_leaf)
    return x;
  return min_leaf_node_in_subtree(x->p[0]);
}

Node *Btree::max_leaf_node_in_subtree(Node *x) {
  if (x->is_leaf)
    return x;
  return max_leaf_node_in_subtree(x->p[x->key_cnt]);
}

void Btree::merge(Node *x, short idx) {
  mc.node_merge++;
  Node *y = x->p[idx];
  Node *z = x->p[idx + 1];

  // push down x's key to y
  y->keys[y->key_cnt] = x->keys[idx];
  for (unsigned short i = idx + 1; i < x->key_cnt; i++) {
    x->keys[i - 1] = x->keys[i];
    x->p[i] = x->p[i + 1];
  }
  x->key_cnt--;
  y->key_cnt++;

  // move z's all keys to y
  for (unsigned short i = 0; i < z->key_cnt; i++) {
    y->keys[y->key_cnt] = z->keys[i];
    y->p[y->key_cnt] = z->p[i];
    y->key_cnt++;
  }
  y->p[y->key_cnt] = z->p[z->key_cnt];

  nm.return_node(z);
}

bool Btree::delete_key(unsigned long k) {
  if (root->key_cnt == 0)
    return false;

  // If key_cnt of root is 1 and 2 children have less than t-1 key, merge it.
  // Then lower height if root key_cnt becomes 0
  if (root->key_cnt == 1 && root->p[0] && root->p[1] &&
      root->p[0]->key_cnt <= key_min && root->p[1]->key_cnt <= key_min) {
    Node *x = root;
    Node *y = root->p[0];
    merge(x, 0);
    nm.return_node(x);
    root = y;
  }

  // Then delete key
  return delete_key(root, k);
}
bool Btree::delete_key(Node *x, unsigned long k) {
  // 0. find a key or link-position
  short i = find_key_or_right_bound_in_node(x, k);
  if (x->is_leaf) {
    // 1. x is leaf node and there is a 'k', just delete it
    if (x->keys[i].key == k &&
        i < x->key_cnt) { // key found
      for (unsigned short j = i + 1; j < x->key_cnt; j++) {
        x->keys[j - 1] = x->keys[j];
      }
      x->key_cnt--;
      return true;
    }
    return false;
  }
  if (i < x->key_cnt &&
      x->keys[i].key == k) { // key found
    if (x->p[i]->key_cnt > key_min) {
      // 2.a x has k and max_leaf_node_in_subtree has more than t-1 keys
      Node *max_leaf = max_leaf_node_in_subtree(x->p[i]);
      x->keys[i] = max_leaf->keys[max_leaf->key_cnt - 1];
      return delete_key(x->p[i], x->keys[i].key);
    } else if (x->p[i + 1]->key_cnt > key_min) {
      // 2.b x has k and min_leaf_node_in_subtre has more than t-1 keys
      Node *min_leaf = min_leaf_node_in_subtree(x->p[i + 1]);
      x->keys[i] = min_leaf->keys[0];
      return delete_key(x->p[i + 1], x->keys[i].key);
    } else {
      // 2.p Neither left and right children has less than eq t-1 keys
      merge(x, i);
      return delete_key(x->p[i], k);
    }
  } else { // key not found at this node
    // 3
    // child node has enough keys
    if (x->p[i]->key_cnt > key_min) {
      return delete_key(x->p[i], k);
    }

    Node *a = x->p[i];
    if (i != 0 && x->p[i - 1]->key_cnt > key_min) {
      // 3.a.left
      // move key from p[i-1] via parent node(x)
      Node *b = x->p[i - 1];
      for (unsigned short j = a->key_cnt; j > 0; j--) {
        a->keys[j] = a->keys[j - 1];
        a->p[j + 1] = a->p[j];
      }
      a->p[1] = a->p[0];

      a->keys[0] = x->keys[i - 1];
      x->keys[i - 1] = b->keys[b->key_cnt - 1];
      a->p[0] = b->p[b->key_cnt];
      a->key_cnt++;
      b->key_cnt--;
    } else if (i != x->key_cnt && x->p[i + 1]->key_cnt > key_min) {
      // 3.a.right
      // move key from p[i+1] via parent node(x)
      Node *c = x->p[i + 1];
      a->keys[a->key_cnt] = x->keys[i];
      x->keys[i] = c->keys[0];
      a->p[a->key_cnt + 1] = c->p[0];
      a->key_cnt++;

      for (unsigned short j = 1; j < c->key_cnt; j++) {
        c->keys[j - 1] = c->keys[j];
        c->p[j - 1] = c->p[j];
      }
      c->p[c->key_cnt - 1] = c->p[c->key_cnt];
      c->key_cnt--;
    } else {
      // 3.b
      if (i == x->key_cnt) {
        i--;
      }
      merge(x, i);
    }
    return delete_key(x->p[i], k);
  }
}

void Btree::tree_walk(Node *x, vector<Item> *v) {
  if (x->is_leaf) {
    for (short i = 0; i < x->key_cnt; i++) {
      v->push_back(x->keys[i]);
      // printf("key: %2lld, val: %2lld\n", x->keys[i].key, x->keys[i].val);
    }
    return;
  } else {
    for (short i = 0; i < x->key_cnt; i++) {
      tree_walk(x->p[i], v);
      v->push_back(x->keys[i]);
      // printf("key: %2lld, val: %2lld\n", x->keys[i].key, x->keys[i].val);
    }
    tree_walk(x->p[x->key_cnt], v);
  }
}
