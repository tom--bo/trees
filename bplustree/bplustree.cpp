#include "bplustree.h"
#include <bits/stdc++.h>
using namespace std;

/*
 * Bplustree
 */
short find_left_most_key_or_right_bound_in_node(BpNode *x, unsigned long k) {
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

short find_right_most_key_or_left_bound_in_node(BpNode *x, unsigned long k) {
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

short find_right_most_pointer_in_node(BpNode *x, unsigned long k) {
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

Bplustree::Bplustree(short t_num, unsigned int l) {
  mc = MetricCounter();
  if (l == 0) {
    nm = new NodeManager(t_num, 3000, B_PLUS);
  } else {
    nm = new LRUNodeManager(t_num, l, B_PLUS);
  }
  key_max = t_num * 2;
  key_min = t_num - 1;

  BpNode *n = allocate_node();
  n->is_leaf = true;
  root = n;
}

// Allocate-BpNode
BpNode *Bplustree::allocate_node() {
  mc.node_count++;
  return (BpNode *)nm->create_node();
}

// insert
void Bplustree::insert(Item k) {
  BpNode *r = root;
  if (r->key_cnt == key_max) {
    BpNode *s = allocate_node();
    root = s;
    s->p[0] = r;
    split_child(s, 0);
  }
  insert_nonfull(root, k);
}

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
    else
      return false;
  } else {
    i = find_left_most_key_or_right_bound_in_node(x, k.key);

    // if child node is full
    nm->check_node(x->p[i]);
    if (x->p[i]->key_cnt == key_max) {
      split_child(x, i);
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
    return false;
  }
}

void Bplustree::split_child(BpNode *x, short i) {
  mc.node_split++;
  BpNode *z = allocate_node();
  nm->check_node(x->p[i]);
  BpNode *y = x->p[i];
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
      z->keys[j] = y->keys[j + key_min + 2];
    }
    z->key_cnt = move_cnt;
    // move second half pointer if y is not leaf-node
    for (unsigned short j = 0; j < move_cnt + 1; j++) {
      z->p[j] = y->p[j + key_min + 2];
      y->p[j + key_min + 2] = nullptr;
    }
  }

  // make key space to add center-key of y
  for (unsigned short j = x->key_cnt; j > i; j--) {
    x->keys[j] = x->keys[j - 1];
    x->p[j + 1] = x->p[j];
  }
  x->p[i + 1] = z;
  // set x->keys[i] min item in z-root-subtree
  x->keys[i] = min_item_in_subtree(x->p[i + 1]);
  y->key_cnt = key_min + 1;
  x->key_cnt++;
}

Item Bplustree::search(BpNode *x, unsigned long k) {
  short l = find_left_most_key_or_right_bound_in_node(x, k);
  short r = find_right_most_key_or_left_bound_in_node(x, k);

  for (short i = l; i <= r + 1 && i <= x->key_cnt; i++) {
    if (i < x->key_cnt && k == x->keys[i].key) {
      return x->keys[i];
    } else if (x->is_leaf) {
      continue;
    }
    nm->check_node(x->p[i]);
    Item it = search(x->p[i], k);
    if (it.key != 0 && it.val != 0) {
      return it;
    }
  }
  return Item{0, 0}; // return (key: 0, val: 0)
}

unsigned long Bplustree::count_range(BpNode *x, unsigned long min_,
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
  nm->check_node(x->p[rp]);
  cnt += count_range(x->p[rp], min_, max_);
  return cnt;
}

BpNode *Bplustree::min_leaf_node_in_subtree(BpNode *x) {
  if (x->is_leaf)
    return x;
  nm->check_node(x->p[0]);
  return min_leaf_node_in_subtree(x->p[0]);
}

Item Bplustree::min_item_in_subtree(BpNode *x) {
  if (x->is_leaf)
    return x->keys[0];
  nm->check_node(x->p[0]);
  return min_item_in_subtree(x->p[0]);
}

BpNode *Bplustree::max_leaf_node_in_subtree(BpNode *x) {
  if (x->is_leaf)
    return x;
  return max_leaf_node_in_subtree(x->p[x->key_cnt]);
}

void Bplustree::merge(BpNode *x, short idx) {
  mc.node_merge++;
  BpNode *y = x->p[idx];
  BpNode *z = x->p[idx + 1];
  nm->check_node(y);
  nm->check_node(z);

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

  nm->return_node((Inode *)z);
  mc.node_count--;
}

bool Bplustree::delete_key(unsigned long k) {
  if (root->key_cnt == 0)
    return false;

  // If key_cnt of root is 1 and 2 children have less than t key, merge it.
  // Then lower height if root key_cnt becomes 0
  if (root->key_cnt == 1 && root->p[0] && root->p[1] &&
      root->p[0]->key_cnt <= key_min && root->p[1]->key_cnt <= key_min) {
    BpNode *x = root;
    BpNode *y = root->p[0];
    merge(x, 0);
    nm->return_node((Inode *)x);
    mc.node_count--;
    root = y;
  }

  // Then delete key
  bool ret = delete_key(root, k);
  return ret;
}

bool Bplustree::delete_key(BpNode *x, unsigned long k) {
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
  nm->check_node(x->p[i]);
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
  BpNode *a = x->p[i];
  nm->check_node(x->p[i - 1]);
  nm->check_node(x->p[i + 1]);
  if (i != 0 && x->p[i - 1]->key_cnt > key_min) { // check left side of p[i]
    // move key from p[i-1]
    //    x
    //  /   \
    // b     a
    BpNode *b = x->p[i - 1];

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
    BpNode *c = x->p[i + 1];
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

void Bplustree::update_metric() {
  // height
  BpNode *x = this->root;
  mc.height = 1;
  while (!x->is_leaf) {
    x = x->p[0];
    mc.height += 1;
  }

  // node count & filling rate
  x = this->root;
  tree_walk_for_metric(x);
}

void Bplustree::tree_walk_for_metric(BpNode *x) {
  if (x->is_leaf) {
    mc.leaf_node_cnt += 1;
    mc.leaf_node_keys_cnt += x->key_cnt;
    return;
  } else {
    if (x != root) {
      mc.intermediate_node_cnt += 1;
      mc.intermediate_node_keys_cnt += x->key_cnt;
    }
    for (short i = 0; i < x->key_cnt; i++) {
      tree_walk_for_metric(x->p[i]);
    }
    tree_walk_for_metric(x->p[x->key_cnt]);
  }
}
void Bplustree::tree_walk(BpNode *x, vector<Item> *v) {
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
