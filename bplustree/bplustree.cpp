#include "bplustree.h"
#include <bits/stdc++.h>
using namespace std;

/*
 * BplustreeNodeManager
 */

BplustreeNodeManager::BplustreeNodeManager() {}
BplustreeNodeManager::BplustreeNodeManager(short t_num, int node_cnt)
    : t{t_num} {
  pool_cnt = 0;
  node_pool = std::vector<BpNode *>(node_cnt);
}

// ok
BpNode *BplustreeNodeManager::get_node() {
  BpNode *n;
  if (!returned_queue.empty()) {
    n = returned_queue.front();
    returned_queue.pop();
    return n;
  }
  if (pool_cnt != node_pool.size() - 1) {
    n = new BpNode(t);
    node_pool.push_back(n);
    pool_cnt++;
    return n;
  }
  return node_pool[pool_cnt++];
}

// ok
void BplustreeNodeManager::return_node(BpNode *n) {
  n->key_cnt = 0;
  n->is_leaf = false;
  n->height = 0;
  n->left = nullptr;
  n->right = nullptr;
  returned_queue.push(n);
}

/*
 * Bplustree
 */

// ok
short find_key_or_right_bound_in_node(BpNode *x, unsigned long k) {
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

// ok
Bplustree::Bplustree(short t_num) : t{t_num} {
  key_max = 2 * t_num;
  key_min = t_num - 1;
  nm = BplustreeNodeManager(t_num, 3000);
  mc = MetricCounter();

  BpNode *n = allocate_node();
  n->is_leaf = true;
  root = n;
}

// Allocate-BpNode
// ok
BpNode *Bplustree::allocate_node() { return nm.get_node(); }

// insert
// ok
void Bplustree::insert(Item k) {
  BpNode *r = root;
  if (r->key_cnt == key_max) {
    BpNode *s = allocate_node();
    root = s;
    s->c[0] = r;
    split_child(s, 0);
  }
  insert_nonfull(root, k);
}

// ok
bool Bplustree::insert_nonfull(BpNode *x, Item k) {
  short i = x->key_cnt;
  if (x->is_leaf) {
    while (i >= 1 && k.key < x->keys[i - 1].key) {
      x->keys[i] = x->keys[i - 1];
      i--;
    }
    x->keys[i] = k;
    x->key_cnt++;
    // if left most in node was changed notify it to parent node
    if (i == 0)
      return true;
  } else {
    i = find_key_or_right_bound_in_node(x, k.key);

    // if child node is full
    if (x->c[i]->key_cnt == key_max) {
      split_child(x, i);
      if (k.key > x->keys[i].key) {
        i++;
      }
    }
    // if left most in child node changed, need to update key
    if (insert_nonfull(x->c[i], k) && i != 0) {
      x->keys[i - 1] = min_item_in_subtree(x->c[i]);
    }
    if (i == 1)
      return true;
  }
  return false;
}

// ok
void Bplustree::split_child(BpNode *x, short i) {
  mc.node_split++;
  BpNode *z = allocate_node();
  BpNode *y = x->c[i];
  z->is_leaf = y->is_leaf;
  // change side-links
  if (y->right) {
    z->right = y->right;
    y->right->left = z;
  }
  z->left = y;
  y->right = z;

  unsigned short move_cnt = 0;
  if (y->is_leaf) { // if leaf node
    // move second-half nodes in y
    move_cnt = key_min + 1;
    for (unsigned short j = 0; j < move_cnt; j++) {
      z->keys[j] = y->keys[j + move_cnt];
    }
    z->key_cnt = move_cnt;

  } else { // if NOT leaf node
    // move (second-half - 1) nodes in y
    move_cnt = key_min;
    for (unsigned short j = 0; j < move_cnt; j++) {
      z->keys[j] = y->keys[j + t + 1];
    }
    z->key_cnt = move_cnt;
    // move second half pointer if y is not leaf-node
    for (unsigned short j = 0; j < move_cnt + 1; j++) {
      z->c[j] = y->c[j + t + 1];
      y->c[j + t + 1] = nullptr;
    }
  }

  // make key space to add center-key of y
  for (unsigned short j = x->key_cnt; j > i; j--) {
    x->keys[j] = x->keys[j - 1];
    x->c[j + 1] = x->c[j];
  }
  x->c[i + 1] = z;
  // set x->keys[i] min item in z-root-subtree
  x->keys[i] = min_item_in_subtree(x->c[i + 1]);
  y->key_cnt = key_min + 1;
  x->key_cnt++;
}

// ok
Item Bplustree::search(BpNode *x, unsigned long k) {
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
  } else if (x->is_leaf || !x->c[i]) {
    return Item{0, 0}; // return (key: 0, val: 0)
  }
  return search(x->c[i], k);
}

// ok
unsigned long Bplustree::count(BpNode *x, unsigned long k) {
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
  cnt += count(x->c[i], k);
  while (i < x->key_cnt && x->keys[i].key == k) {
    i++;
    cnt += count(x->c[i], k);
  }
  return cnt;
}

// ok
BpNode *Bplustree::min_leaf_node_in_subtree(BpNode *x) {
  if (x->is_leaf)
    return x;
  return min_leaf_node_in_subtree(x->c[0]);
}

// ok
Item Bplustree::min_item_in_subtree(BpNode *x) {
  if (x->is_leaf)
    return x->keys[0];
  return min_item_in_subtree(x->c[0]);
}

// ok
BpNode *Bplustree::max_leaf_node_in_subtree(BpNode *x) {
  if (x->is_leaf)
    return x;
  return max_leaf_node_in_subtree(x->c[x->key_cnt]);
}

// ok
void Bplustree::merge(BpNode *x, short idx) {
  mc.node_merge++;
  BpNode *y = x->c[idx];
  BpNode *z = x->c[idx + 1];
  // cout << idx << endl;

  // cout << "merge called" << endl;
  // cout << "-- x --" << endl;
  // for (unsigned short j = 0; j < x->key_cnt; j++) {
  //   cout << x->keys[j].key << " ";
  // }
  // cout << endl;

  // cout << "-- y --" << endl;
  // for (unsigned short j = 0; j < y->key_cnt; j++) {
  //   cout << y->keys[j].key << " ";
  // }
  // cout << endl;

  // cout << "-- z --" << endl;
  // for (unsigned short j = 0; j < z->key_cnt; j++) {
  //   cout << z->keys[j].key << " ";
  // }
  // cout << endl;

  if (!y->is_leaf) {
    // push down x's key to y
    y->keys[y->key_cnt] = x->keys[idx];
  }

  // delete x's key to merge
  for (unsigned short i = idx + 1; i < x->key_cnt; i++) {
    x->keys[i - 1] = x->keys[i];
    x->c[i] = x->c[i + 1];
  }
  x->key_cnt--;

  // move z's all keys to y
  for (unsigned short i = 0; i < z->key_cnt; i++) {
    y->keys[y->key_cnt] = z->keys[i];
    y->c[y->key_cnt] = z->c[i];
    y->key_cnt++;
  }
  y->c[y->key_cnt] = z->c[z->key_cnt];

  if (z->right) {
    y->right = z->right;
    z->right->left = y;
  }

  nm.return_node(z);
}

// ok
bool Bplustree::delete_key(unsigned long k) {
  if (root->key_cnt == 0)
    return false;

  // If key_cnt of root is 1 and 2 children have less than t key, merge it.
  // Then lower height if root key_cnt becomes 0
  if (root->key_cnt == 1 && root->c[0] && root->c[1] &&
      root->c[0]->key_cnt <= key_min && root->c[1]->key_cnt <= key_min) {
    BpNode *x = root;
    BpNode *y = root->c[0];
    merge(x, 0);
    nm.return_node(x);
    root = y;
  }

  // Then delete key
  bool ret = delete_key(root, k);
  return ret;
}

bool Bplustree::delete_key(BpNode *x, unsigned long k) {
  // find a key or link-position
  short i = find_key_or_right_bound_in_node(x, k);
  if (x->is_leaf) {
    if (x->keys[i].key == k &&
        i < x->key_cnt) { // key found // TODO: 後半の判定消せないか?
      for (unsigned short j = i + 1; j < x->key_cnt; j++) {
        x->keys[j - 1] = x->keys[j];
      }
      x->key_cnt--;

      if (i == 0) {
        return true;
      }
    }
    return false;
  }

  // Not leaf node
  if (x->keys[i].key == k) {
    i++;
  }
  // child has enough keys
  if (x->c[i]->key_cnt > key_min) {
    if (delete_key(x->c[i], k) && i != 0) {
      x->keys[i - 1] = min_item_in_subtree(x->c[i]);
      if (i - 1 == 0) {
        return true;
      }
    }
    return false;
  }
  // child(s) don't have enough key
  BpNode *a = x->c[i];
  if (i != 0 && x->c[i - 1]->key_cnt > key_min) { // check left side of c[i]
    // move key from c[i-1]
    //    x
    //  /   \
    // b     a
    BpNode *b = x->c[i - 1];

    // make space for move key to a
    for (unsigned short j = a->key_cnt; j > 0; j--) {
      a->keys[j] = a->keys[j - 1];
      a->c[j + 1] = a->c[j];
    }
    a->c[1] = a->c[0];

    // move key from b to b
    a->keys[0] = b->keys[b->key_cnt - 1];
    a->c[0] = b->c[b->key_cnt];
    a->key_cnt++;
    b->key_cnt--;

    x->keys[i - 1] = min_item_in_subtree(x->c[i]);
  } else if (i != x->key_cnt && x->c[i + 1]->key_cnt > key_min) {
    // move key from c[i+1]
    //    x
    //  /   \
    // a     c
    BpNode *c = x->c[i + 1];
    a->keys[a->key_cnt] = c->keys[0];
    a->c[a->key_cnt + 1] = c->c[0];
    a->key_cnt++;

    for (unsigned short j = 1; j < c->key_cnt; j++) {
      c->keys[j - 1] = c->keys[j];
      c->c[j - 1] = c->c[j];
    }
    c->c[c->key_cnt - 1] = c->c[c->key_cnt];
    c->key_cnt--;
    x->keys[i] = min_item_in_subtree(x->c[i + 1]);
  } else {
    if (i > 0) {
      i--;
    }
    merge(x, i);
    // cout << "merged" << endl;
    // for (unsigned short j = 0; j < x->c[i]->key_cnt; j++) {
    //   cout << x->c[i]->keys[j].key << " ";
    // }
    // cout << endl;
    // for (unsigned short j = 0; j < x->c[i+1]->key_cnt; j++) {
    //   cout << x->c[i+1]->keys[j].key << " ";
    // }
    // cout << endl;
  }
  delete_key(x->c[i], k);
  return true;
}

// ok
void Bplustree::tree_walk(BpNode *x, vector<Item> *v) {
  if (x->is_leaf) {
    for (short i = 0; i < x->key_cnt; i++) {
      v->push_back(x->keys[i]);
      // printf("key: %2lld, val: %2lld\n", x->keys[i].key, x->keys[i].val);
    }
    return;
  } else {
    for (short i = 0; i < x->key_cnt; i++) {
      tree_walk(x->c[i], v);
      // printf("key: %2lld, val: %2lld\n", x->keys[i].key, x->keys[i].val);
    }
    tree_walk(x->c[x->key_cnt], v);
  }
}
