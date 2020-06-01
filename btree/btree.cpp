#include "btree.h"
#include <bits/stdc++.h>
using namespace std;

Btree::Btree(unsigned short t_num) : t{t_num} {
  key_max = 2 * t_num - 1;
  key_min = t_num - 1;
  Node *n = allocate_node();
  n->is_leaf = true;
  root = n;
}

// Allocate-Node
Node *Btree::allocate_node() {
  Node *n = new Node(t);
  return n;
}

// insert
void Btree::insert(Item k) {
  Node *r = root;
  if (r->key_cnt == key_max) {
    Node *s = allocate_node();
    root = s;
    s->c[0] = r;
    split_child(s, 0);
  }
  insert_nonfull(root, k);
}

void Btree::insert_nonfull(Node *x, Item k) {
  unsigned short i = x->key_cnt;
  if (x->is_leaf) {
    while (i >= 1 && k.key < x->keys[i - 1].key) {
      x->keys[i] = x->keys[i - 1];
      i--;
    }
    x->keys[i] = k;
    x->key_cnt++;
  } else {
    while (i >= 1 && k.key < x->keys[i - 1].key) {
      i--;
    }

    // if child node is full
    if (x->c[i]->key_cnt == key_max) {
      split_child(x, i);
      if (k.key > x->keys[i].key) {
        i++;
      }
    }
    insert_nonfull(x->c[i], k);
  }
}

void Btree::split_child(Node *x, unsigned short i) {
  Node *z = allocate_node();
  Node *y = x->c[i];
  z->is_leaf = y->is_leaf;

  // move second half nodes in y
  for (unsigned short j = 0; j < key_min; j++) {
    z->keys[j] = y->keys[j + t];
  }
  z->key_cnt = key_min;
  // move second half pinter if y is not leaf-node
  if (!y->is_leaf) {
    for (unsigned short j = 0; j < t; j++) {
      z->c[j] = y->c[j + t];
      y->c[j + t] = nullptr;
    }
  }

  // make key space to add center-key of y
  for (unsigned short j = x->key_cnt; j > i; j--) {
    x->keys[j] = x->keys[j - 1];
    x->c[j + 1] = x->c[j];
  }
  // move up center-key of y
  x->c[i + 1] = z;
  x->keys[i] = y->keys[t - 1];
  y->key_cnt = key_min;
  x->key_cnt++;
}

Item Btree::search(Node *x, unsigned long k) {
  unsigned short i = 0;
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

Node *Btree::min_leaf_node_in_subtree(Node *x) {
  if (x->is_leaf)
    return x;
  return min_leaf_node_in_subtree(x->c[0]);
}

Node *Btree::max_leaf_node_in_subtree(Node *x) {
  if (x->is_leaf)
    return x;
  return max_leaf_node_in_subtree(x->c[x->key_cnt]);
}

void Btree::merge(Node *x, unsigned short idx) {
  Node *y = x->c[idx];
  Node *z = x->c[idx + 1];

  // push down x's key to y
  y->keys[y->key_cnt] = x->keys[idx];
  for (unsigned short i = idx + 1; i < x->key_cnt; i++) {
    x->keys[i - 1] = x->keys[i];
    x->c[i] = x->c[i + 1];
  }
  x->key_cnt--;
  y->key_cnt++;

  // move z's all keys to y
  for (unsigned short i = 0; i < z->key_cnt; i++) {
    y->keys[y->key_cnt] = z->keys[i];
    y->c[y->key_cnt] = z->c[i];
    y->key_cnt++;
  }
  y->c[y->key_cnt] = z->c[z->key_cnt];

  delete z;
}

bool Btree::delete_key(unsigned long k) {
  if (root->key_cnt == 0)
    return false;

  // If key_cnt of root is 1 and 2 children have less than t-1 key, merge it.
  // Then lower height if root key_cnt becomes 0
  if (root->key_cnt == 1 && root->c[0] && root->c[1] &&
      root->c[0]->key_cnt <= key_min && root->c[1]->key_cnt <= key_min) {
    Node *x = root;
    Node *y = root->c[0];
    merge(x, 0);
    delete x;
    root = y;
  }

  // Then delete key
  return delete_key(root, k);
}
bool Btree::delete_key(Node *x, unsigned long k) {
  // 0. find a key or link-position
  unsigned short i = x->key_cnt - 1;
  while (i > 0 && x->keys[i].key > k) {
    i--;
  }
  if (x->keys[i].key < k)
    i++;
  if (x->is_leaf) {
    // 1. x is leaf node and there is a 'k', just delete it
    if (x->keys[i].key == k &&
        i < x->key_cnt) { // key found // TODO: 後半の判定消せないか?
      for (unsigned short j = i + 1; j < x->key_cnt; j++) {
        x->keys[j - 1] = x->keys[j];
      }
      x->key_cnt--;
      return true;
    }
    return false;
  }
  if (i < x->key_cnt &&
      x->keys[i].key == k) { // key found // TODO: 前半の判定消せないか?
    if (x->c[i]->key_cnt > key_min) {
      // 2.a x has k and max_leaf_node_in_subtree has more than t-1 keys
      Node *max_leaf = max_leaf_node_in_subtree(x->c[i]);
      x->keys[i] = max_leaf->keys[max_leaf->key_cnt - 1];
      return delete_key(x->c[i], x->keys[i].key);
    } else if (x->c[i + 1]->key_cnt > key_min) {
      // 2.b x has k and min_leaf_node_in_subtre has more than t-1 keys
      Node *min_leaf = min_leaf_node_in_subtree(x->c[i + 1]);
      x->keys[i] = min_leaf->keys[0];
      return delete_key(x->c[i + 1], x->keys[i].key);
    } else {
      // 2.c Neither left and right children has less than eq t-1 keys
      merge(x, i);
      return delete_key(x->c[i], k);
    }
  } else { // key not found at this node
    // 3
    // child node has enough keys
    if (x->c[i]->key_cnt > key_min) {
      return delete_key(x->c[i], k);
    }

    Node *a = x->c[i];
    if (i != 0 && x->c[i - 1]->key_cnt > key_min) {
      // 3.a.left
      // move key from c[i-1] via parent node(x)
      Node *b = x->c[i - 1];
      for (unsigned short j = a->key_cnt; j > 0; j--) {
        a->keys[j] = a->keys[j - 1];
        a->c[j + 1] = a->c[j];
      }
      a->c[1] = a->c[0];

      a->keys[0] = x->keys[i - 1];
      x->keys[i - 1] = b->keys[b->key_cnt - 1];
      a->c[0] = b->c[b->key_cnt];
      a->key_cnt++;
      b->key_cnt--;
    } else if (i != x->key_cnt && x->c[i + 1]->key_cnt > key_min) {
      // 3.a.right
      // move key from c[i+1] via parent node(x)
      Node *c = x->c[i + 1];
      a->keys[a->key_cnt] = x->keys[i];
      x->keys[i] = c->keys[0];
      a->c[a->key_cnt + 1] = c->c[0];
      a->key_cnt++;

      for (unsigned short j = 1; j < c->key_cnt; j++) {
        c->keys[j - 1] = c->keys[j];
        c->c[j - 1] = c->c[j];
      }
      c->c[c->key_cnt - 1] = c->c[c->key_cnt];
      c->key_cnt--;
    } else {
      // 3.b
      if (i == x->key_cnt) {
        i--;
      }
      merge(x, i);
    }
    return delete_key(x->c[i], k);
  }
}

void Btree::tree_walk(Node *x, vector<Item> *v) {
  if (x->is_leaf) {
    for (unsigned short i = 0; i < x->key_cnt; i++) {
      v->push_back(x->keys[i]);
      // printf("key: %2lld, val: %2lld\n", x->keys[i].key, x->keys[i].val);
    }
    return;
  } else {
    for (unsigned short i = 0; i < x->key_cnt; i++) {
      tree_walk(x->c[i], v);
      v->push_back(x->keys[i]);
      // printf("key: %2lld, val: %2lld\n", x->keys[i].key, x->keys[i].val);
    }
    tree_walk(x->c[x->key_cnt], v);
  }
}
