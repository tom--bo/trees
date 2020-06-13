#include "bastertree.h"
#include <bits/stdc++.h>
using namespace std;

/*
 * BastertreeNodeManager
 */

BastertreeNodeManager::BastertreeNodeManager(short t_num, int node_cnt)
    : t{t_num} {
  pool_cnt = 0;
  node_pool = std::vector<BaNode *>(node_cnt);
}

BaNode *BastertreeNodeManager::get_node() {
  BaNode *n;
  if (!returned_queue.empty()) {
    n = returned_queue.front();
    returned_queue.pop();
    return n;
  }
  if (pool_cnt != node_pool.size() - 1) {
    n = new BaNode(t);
    node_pool.push_back(n);
    pool_cnt++;
    return n;
  }
  return node_pool[pool_cnt++];
}

void BastertreeNodeManager::return_node(BaNode *n) {
  n->key_cnt = 0;
  n->is_leaf = false;
  n->height = 0;
  n->left = nullptr;
  n->right = nullptr;
  returned_queue.push(n);
}

/*
 * Bastertree
 */
short find_left_most_key_or_right_bound_in_node(BaNode *x, unsigned long k) {
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

short find_right_most_key_or_left_bound_in_node(BaNode *x, unsigned long k) {
  short l = -1, r = x->key_cnt, m;
  while (r - l > 1) {
    m = (l + r) / 2;
    if (x->keys[m].key > k) {
      r = m;
    } else {
      l = m;
    }
  }
  return l;
}

short find_right_most_pointer_in_node(BaNode *x, unsigned long k) {
  short l = 0, r = x->key_cnt, m;
  while (r - l > 1) {
    m = (r + l) / 2;
    if (x->keys[m].key <= k) {
      l = m;
    } else {
      r = m;
    }
  }
  if (l < x->key_cnt && x->keys[l].key <= k) {
    l++;
  }
  return l;
}

Bastertree::Bastertree(short t_num)
    : t{t_num}, nm{BastertreeNodeManager(t_num, 3000)} {
  key_max = t_num * 2;
  key_min = t_num * 2 / 3;
  nm = BastertreeNodeManager(t_num, 3000);
  mc = BaMetricCounter();

  BaNode *n = allocate_node();
  n->is_leaf = true;
  root = n;
}

// Allocate-BaNode
BaNode *Bastertree::allocate_node() { return nm.get_node(); }

// insert
void Bastertree::insert(Item k) {
  BaNode *r = root;
  if (r->key_cnt == key_max) {
    BaNode *s = allocate_node();
    root = s;
    s->p[0] = r;
    split_root(s, 0);
  }
  insert_nonfull(root, k);
}

bool Bastertree::insert_nonfull(BaNode *x, Item k) {
  short i = x->key_cnt;
  bool splitted_flag = false;
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
    else
      return false;
  } else {
    i = find_left_most_key_or_right_bound_in_node(x, k.key);

    // if child node is full
    if (x->p[i]->key_cnt == key_max) {
      splitted_flag = true;
      if (i != 0) {
        if (x->p[i - 1]->key_cnt == key_max) {
          split_child(x, i);
        } else {
          // todo: move key to i-1 node
          splitted_flag = true;
        }
      } else {
        if (x->p[i + 1]->key_cnt == key_max) {
          split_child(x, i + 1);
        } else {
          // todo: move key to i+1 node
        }
      }
      if (k.key > x->keys[i].key) {
        i++;
      }
    }
    // if left most in child node changed, need to update key
    if (insert_nonfull(x->p[i], k)) {
      if (i != 0) {
        x->keys[i - 1] = min_item_in_subtree(x->p[i]);
      }
      return true;
    }
    return splitted_flag;
  }
}

void Bastertree::split_root(BaNode *x, short i) {
  mc.node_split++;
  BaNode *z = allocate_node();
  BaNode *y = x->p[i];
  z->is_leaf = true;
  z->left = y;
  y->right = z;

  unsigned short move_cnt = 0;
  // move key_min nodes in y
  move_cnt = key_min + 1;
  for (unsigned short j = 0; j < move_cnt; j++) {
    z->keys[j] = y->keys[j + move_cnt];
  }
  z->key_cnt = move_cnt;
  y->key_cnt -= move_cnt;

  x->p[1] = z;
  x->keys[0] = min_item_in_subtree(x->p[1]);
  x->key_cnt++;
}

void Bastertree::split_child(BaNode *x, short i) {
  mc.node_split++;
  BaNode *z = allocate_node();
  BaNode *y = x->p[i];
  BaNode *ly = x->p[i - 1];
  z->is_leaf = y->is_leaf;
  // change side-links
  if (y->right) {
    z->right = y->right;
    y->right->left = z;
  }
  z->left = y;
  y->right = z;

  // move key_min*2 from y to z
  unsigned short move_cnt = 0;
  if (y->is_leaf) { // if leaf node
    // move second-half nodes in y
    move_cnt = key_min * 2;
    for (unsigned short j = 0; j < move_cnt; j++) {
      z->keys[j] = y->keys[j + move_cnt];
    }
    z->key_cnt = move_cnt;
    y->key_cnt -= move_cnt;

  } else { // if NOT leaf node
    // move (second-half - 1) nodes in y
    move_cnt = key_min * 2 - 1;
    for (unsigned short j = 0; j < move_cnt; j++) {
      z->keys[j] = y->keys[j + t + 1];
    }
    // move second half pointer if y is not leaf-node
    for (unsigned short j = 0; j < move_cnt + 1; j++) {
      z->p[j] = y->p[j + t + 1];
      y->p[j + t + 1] = nullptr;
    }
    z->key_cnt += move_cnt;
    y->key_cnt -= move_cnt + 1;
  }

  // move key_min from ly to y
  if (y->is_leaf) { // if leaf node
    // move key_min nodes in ly
    move_cnt = key_min;
    for (unsigned short j = y->key_cnt - 1; j >= 0; j--) {
      y->keys[j + move_cnt] = y->keys[j];
    }
    for (unsigned short j = 0; j < move_cnt; j++) {
      y->keys[j] = ly->keys[ly->key_cnt - move_cnt + j];
    }
    y->key_cnt += move_cnt;
    ly->key_cnt -= move_cnt;

  } else { // if NOT leaf node
    // move key_min nodes in ly

    move_cnt = key_min;
    for (unsigned short j = y->key_cnt - 1; j >= 0; j--) {
      y->keys[j + move_cnt] = y->keys[j];
      y->p[j + 1 + move_cnt] = y->p[j + 1];
    }
    y->p[1] = y->p[0];

    for (unsigned short j = 0; j < move_cnt; j++) {
      y->keys[j] = ly->keys[ly->key_cnt - move_cnt + j];
      y->p[j + 1] = ly->p[ly->key_cnt - move_cnt + j + 1];
    }
    y->p[0] = ly->p[ly->key_cnt - move_cnt];
    y->key_cnt += move_cnt;
    ly->key_cnt -= move_cnt;
  }

  // make key space to add center-key of y
  for (unsigned short j = x->key_cnt; j > i; j--) {
    x->keys[j] = x->keys[j - 1];
    x->p[j + 1] = x->p[j];
  }
  x->p[i + 1] = z;
  // set x->keys[i] min item in z-root-subtree
  x->keys[i] = min_item_in_subtree(x->p[i + 1]);
  x->keys[i - 1] = min_item_in_subtree(x->p[i]);

  x->key_cnt++;
}

Item Bastertree::search(BaNode *x, unsigned long k) {
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

/*
unsigned long Bastertree::count_full_scan(BaNode *x, unsigned long k) {
  unsigned long cnt = 0;
  if (x->is_leaf) {
    for (short i = 0; i < x->key_cnt; i++) {
      if (x->keys[i].key == k) {
        cnt++;
      }
    }
    if (x->right) {
      cnt += count(x->right, k);
    }
    return cnt;
  }
  cnt += count(x->p[0], k);
  return cnt;
}
*/

unsigned long Bastertree::count_range(BaNode *x, unsigned long min_,
                                      unsigned long max_) {
  unsigned long cnt = 0;
  if (x->is_leaf) {
    short l = find_left_most_key_or_right_bound_in_node(x, min_);
    short r = find_right_most_key_or_left_bound_in_node(x, max_);
    cnt += r - l + 1;
    if (x->left && cnt != 0) {
      cnt += count_range(x->left, min_, max_);
    }
    return cnt;
  }
  short rp = find_right_most_pointer_in_node(x, max_);
  cnt += count_range(x->p[rp], min_, max_);
  return cnt;
}

BaNode *Bastertree::min_leaf_node_in_subtree(BaNode *x) {
  if (x->is_leaf)
    return x;
  return min_leaf_node_in_subtree(x->p[0]);
}

Item Bastertree::min_item_in_subtree(BaNode *x) {
  if (x->is_leaf)
    return x->keys[0];
  return min_item_in_subtree(x->p[0]);
}

BaNode *Bastertree::max_leaf_node_in_subtree(BaNode *x) {
  if (x->is_leaf)
    return x;
  return max_leaf_node_in_subtree(x->p[x->key_cnt]);
}

void Bastertree::merge(BaNode *x, short idx) {
  mc.node_merge++;
  BaNode *y = x->p[idx];
  BaNode *z = x->p[idx + 1];

  // delete x's key to merge
  for (unsigned short i = idx + 1; i < x->key_cnt; i++) {
    x->keys[i - 1] = x->keys[i];
    x->p[i] = x->p[i + 1];
  }
  x->key_cnt--;

  // move z's all keys to y
  // unless child nodes are't, new mid-key needed before merge
  if (!y->is_leaf) {
    y->keys[y->key_cnt] = min_item_in_subtree(z->p[0]);
    y->key_cnt++;
  }

  for (unsigned short i = 0; i < z->key_cnt; i++) {
    y->keys[y->key_cnt] = z->keys[i];
    y->p[y->key_cnt] = z->p[i];
    y->key_cnt++;
  }
  y->p[y->key_cnt] = z->p[z->key_cnt];

  if (z->right) {
    y->right = z->right;
    z->right->left = y;
  } else {
    y->right = nullptr;
  }

  nm.return_node(z);
}

bool Bastertree::delete_key(unsigned long k) {
  if (root->key_cnt == 0)
    return false;

  // If key_cnt of root is 1 and 2 children have less than t key, merge it.
  // Then lower height if root key_cnt becomes 0
  if (root->key_cnt == 1 && root->p[0] && root->p[1] &&
      root->p[0]->key_cnt <= key_min && root->p[1]->key_cnt <= key_min) {
    BaNode *x = root;
    BaNode *y = root->p[0];
    merge(x, 0);
    nm.return_node(x);
    root = y;
  }

  // Then delete key
  bool ret = delete_key(root, k);
  return ret;
}

bool Bastertree::delete_key(BaNode *x, unsigned long k) {
  // find a key or link-position
  short i = find_left_most_key_or_right_bound_in_node(x, k);
  if (x->is_leaf) {
    if (x->keys[i].key == k && i < x->key_cnt) { // key found
      for (unsigned short j = i; j < x->key_cnt - 1; j++) {
        x->keys[j] = x->keys[j + 1];
      }
      x->key_cnt--;
      if (i == 0) {
        return true;
      }
    }
    return false;
  }

  // Not leaf node
  if (x->keys[i].key == k && i < x->key_cnt) {
    i++;
  }
  // child has enough keys
  if (x->p[i]->key_cnt > key_min) {
    if (delete_key(x->p[i], k)) {
      if (i != 0) {
        x->keys[i - 1] = min_item_in_subtree(x->p[i]);
      }
      return true;
    }
    return false;
  }
  // child(s) don't have enough key
  BaNode *a = x->p[i];
  if (i != 0 && x->p[i - 1]->key_cnt > key_min) { // check left side of p[i]
    // move key from p[i-1]
    //    x
    //  /   \
    // b     a
    BaNode *b = x->p[i - 1];

    // make space for move key to a
    for (unsigned short j = a->key_cnt; j > 0; j--) {
      a->keys[j] = a->keys[j - 1];
      a->p[j + 1] = a->p[j];
    }
    a->p[1] = a->p[0];

    // move key from b to a (only link and need to re-calculate key[0]
    if (a->is_leaf) {
      a->keys[0] = b->keys[b->key_cnt - 1];
    } else {
      a->keys[0] = min_item_in_subtree(a->p[1]);
    }
    a->p[0] = b->p[b->key_cnt];
    a->key_cnt++;
    b->key_cnt--;

    x->keys[i - 1] = min_item_in_subtree(x->p[i]);
  } else if (i != x->key_cnt && x->p[i + 1]->key_cnt > key_min) {
    // move key from p[i+1]
    //    x
    //  /   \
    // a     c
    BaNode *c = x->p[i + 1];
    // need to re-calculate a's last key
    if (a->is_leaf) {
      a->keys[a->key_cnt] = c->keys[0];
    } else {
      a->keys[a->key_cnt] = min_item_in_subtree(c->p[0]);
    }
    a->p[a->key_cnt + 1] = c->p[0];
    a->key_cnt++;

    for (unsigned short j = 1; j < c->key_cnt; j++) {
      c->keys[j - 1] = c->keys[j];
      c->p[j - 1] = c->p[j];
    }
    c->p[c->key_cnt - 1] = c->p[c->key_cnt];
    c->key_cnt--;
    x->keys[i] = min_item_in_subtree(x->p[i + 1]);
  } else {
    if (i > 0) {
      i--;
    }
    merge(x, i);
  }
  if (delete_key(x->p[i], k)) {
    if (i != 0) {
      x->keys[i - 1] = min_item_in_subtree(x->p[i]);
    }
  }
  return true;
}

void Bastertree::tree_walk(BaNode *x, vector<Item> *v) {
  if (x->is_leaf) {
    for (short i = 0; i < x->key_cnt; i++) {
      v->push_back(x->keys[i]);
      // printf("key: %2lld, val: %2lld\n", x->keys[i].key, x->keys[i].val);
    }
    return;
  } else {
    for (short i = 0; i < x->key_cnt + 1; i++) {
      tree_walk(x->p[i], v);
      // printf("key: %2lld, val: %2lld\n", x->keys[i].key, x->keys[i].val);
    }
  }
}
