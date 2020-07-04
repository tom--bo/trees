#include "bstartree.h"
#include <bits/stdc++.h>
using namespace std;

/*
 * Bstartree
 */

short find_left_most_key_or_right_bound_in_node(BsNode *x, unsigned long k) {
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

short find_right_most_key_or_left_bound_in_node(BsNode *x, unsigned long k) {
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

Bstartree::Bstartree(short t_num, unsigned int l) {
  mc = MetricCounter();
  if (l == 0) {
    nm = new NodeManager(t_num, 3000, B_STAR);
  } else {
    nm = new LRUNodeManager(t_num, l, B_STAR);
  }
  key_max = t_num * 2;
  key_min = t_num - 1;
  split_key_min = (t_num * 2 - 1) * 2 / 3;

  BsNode *n = allocate_node();
  n->is_leaf = true;
  root = n;
}

// Allocate-BsNode
BsNode *Bstartree::allocate_node() {
  mc.node_count++;
  return (BsNode *)nm->create_node();
}

// insert
void Bstartree::insert(Item k) {
  BsNode *r = root;
  if (r->key_cnt == key_max) {
    BsNode *s = allocate_node();
    s->is_leaf = false;
    root = s;
    s->p[0] = r;
    split_root(s);
  }
  insert_nonfull(root, k);
}

void Bstartree::insert_nonfull(BsNode *x, Item k) {
  short i = x->key_cnt;
  if (x->is_leaf) {
    while (i >= 1 && k.key < x->keys[i - 1].key) {
      x->keys[i] = x->keys[i - 1];
      i--;
    }
    x->keys[i] = k;
    x->key_cnt++;
  } else {
    i = find_left_most_key_or_right_bound_in_node(x, k.key);
    bool key_moved = false;

    // if child node is full
    if (x->p[i]->key_cnt == key_max) {
      key_moved = true;
      // check left side
      if (i != 0) {
        // if left node is almost full, split
        nm->check_node(x->p[i - 1]);
        if (x->p[i - 1]->key_cnt >= key_max - 1) {
          split_child(x, i);
        } else { // if not almost full move key
          move_key_to_left(x, i);
        }
      } else {
        // if right node is almost full, split
        nm->check_node(x->p[i + 1]);
        if (x->p[i + 1]->key_cnt >= key_max - 1) {
          split_child(x, i + 1);
        } else { // if not almost full move key
          move_key_to_right(x, i);
        }
      }
    }
    // unless the key was not still inserted after split or move_key
    if (key_moved) {
      i = find_left_most_key_or_right_bound_in_node(x, k.key);
    }
    insert_nonfull(x->p[i], k);
  }
}

void Bstartree::move_key_to_left(BsNode *x, short i) {
  BsNode *l = x->p[i - 1];
  BsNode *r = x->p[i];
  nm->check_node(l);
  nm->check_node(r);

  // move p to l
  l->keys[l->key_cnt] = x->keys[i - 1];
  l->p[l->key_cnt + 1] = r->p[0];
  // move up key from r
  x->keys[i - 1] = r->keys[0];

  // slide keys in r
  for (short j = 0; j < r->key_cnt - 1; j++) {
    r->keys[j] = r->keys[j + 1];
    r->p[j] = r->p[j + 1];
  }
  r->p[r->key_cnt - 1] = r->p[r->key_cnt];

  l->key_cnt += 1;
  r->key_cnt -= 1;
}

void Bstartree::move_key_to_right(BsNode *x, short i) {
  BsNode *l = x->p[i];
  BsNode *r = x->p[i + 1];
  nm->check_node(l);
  nm->check_node(r);
  // make space in r
  r->p[r->key_cnt + 1] = r->p[r->key_cnt];
  for (short j = r->key_cnt - 1; j >= 0; j--) {
    r->keys[j + 1] = r->keys[j];
    r->p[j + 1] = r->p[j];
  }
  // move (key, p) to r
  r->keys[0] = x->keys[i];
  r->p[0] = l->p[l->key_cnt];

  // move key up to x from l
  x->keys[i] = l->keys[l->key_cnt - 1];
  l->key_cnt -= 1;
  r->key_cnt += 1;
}

void Bstartree::move_keys_to_right(BsNode *x, short i, short cnt,
                                   bool to_empty) {
  BsNode *l = x->p[i];
  BsNode *r = x->p[i + 1];
  nm->check_node(l);
  nm->check_node(r);
  if (!to_empty) {
    // make space in r
    r->p[r->key_cnt + cnt] = r->p[r->key_cnt];
    for (short j = r->key_cnt - 1; j >= 0; j--) {
      r->keys[j + cnt] = r->keys[j];
      r->p[j + cnt] = r->p[j];
    }
    // push key down from x
    r->keys[cnt - 1] = x->keys[i];
    // move keys from left to right
    r->p[cnt - 1] = l->p[l->key_cnt];
    for (short j = cnt - 1; j >= 1; j--) {
      r->keys[cnt - j - 1] = l->keys[l->key_cnt - j];
      r->p[cnt - j - 1] = l->p[l->key_cnt - j];
    }
    // move key up to x
    x->keys[i] = l->keys[l->key_cnt - cnt];

  } else { // move to empty node
    for (short j = 0; j < cnt; j++) {
      r->keys[j] = l->keys[l->key_cnt - cnt + j];
      r->p[j] = l->p[l->key_cnt - cnt + j];
    }
    r->p[cnt] = l->p[l->key_cnt];
    // insert x key to r
    // move key up to x
    x->keys[i] = l->keys[l->key_cnt - cnt - 1];
    l->key_cnt -= 1;
  }
  l->key_cnt -= cnt;
  r->key_cnt += cnt;
}

void Bstartree::split_root(BsNode *x) {
  //     x
  //   /   \
  //  y     z
  mc.node_split++;
  BsNode *z = allocate_node();
  BsNode *y = x->p[0];
  nm->check_node(y);
  z->is_leaf = y->is_leaf;
  x->p[1] = z;

  // move second half nodes in y
  move_keys_to_right(x, 0, 1, true);
  x->key_cnt = 1;
}

void Bstartree::split_child(BsNode *x, short i) {
  //    x
  //  /   \
  // y     z (new)
  mc.node_split++;
  // slide keys to right
  if (i < x->key_cnt) {
    for (short j = x->key_cnt - 1; j >= i; j--) {
      x->keys[j + 1] = x->keys[j];
      x->p[j + 2] = x->p[j + 1];
    }
  }

  // add z node
  BsNode *y = x->p[i];
  nm->check_node(y);
  BsNode *z = allocate_node();
  z->is_leaf = y->is_leaf;
  x->p[i + 1] = z;

  // move split_key_min nodes in y
  move_keys_to_right(x, i, split_key_min, true);
  x->key_cnt++;

  // move half of split_key_min node in ly
  move_keys_to_right(x, i - 1, split_key_min / 2, false);
}

Item Bstartree::search(BsNode *x, unsigned long k) {
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

unsigned long Bstartree::count_range(BsNode *x, unsigned long min_,
                                     unsigned long max_) {
  unsigned long cnt = 0;
  short l = find_left_most_key_or_right_bound_in_node(x, min_);
  short r = find_right_most_key_or_left_bound_in_node(x, max_);
  for (short i = l; i <= r + 1; i++) {
    if (i < x->key_cnt && x->keys[i].key >= min_ && x->keys[i].key <= max_) {
      cnt++;
    }
    if (!x->is_leaf) {
      nm->check_node(x->p[i]);
      cnt += count_range(x->p[i], min_, max_);
    }
  }
  return cnt;
}

BsNode *Bstartree::min_leaf_node_in_subtree(BsNode *x) {
  if (x->is_leaf)
    return x;
  nm->check_node(x->p[0]);
  return min_leaf_node_in_subtree(x->p[0]);
}

BsNode *Bstartree::max_leaf_node_in_subtree(BsNode *x) {
  if (x->is_leaf)
    return x;
  nm->check_node(x->p[x->key_cnt]);
  return max_leaf_node_in_subtree(x->p[x->key_cnt]);
}

void Bstartree::merge(BsNode *x, short idx) {
  mc.node_merge++;
  BsNode *y = x->p[idx];
  BsNode *z = x->p[idx + 1];
  nm->check_node(y);
  nm->check_node(z);

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

  nm->return_node((Inode *)z);
  mc.node_count--;
}

bool Bstartree::delete_key(unsigned long k) {
  if (root->key_cnt == 0)
    return false;

  // If key_cnt of root is 1 and 2 children have less than t-1 key, merge it.
  // Then lower height if root key_cnt becomes 0
  if (root->key_cnt == 1 && root->p[0] && root->p[1] &&
      root->p[0]->key_cnt <= key_min && root->p[1]->key_cnt <= key_min) {
    BsNode *x = root;
    BsNode *y = root->p[0];
    merge(x, 0);
    nm->return_node((Inode *)x);
    mc.node_count--;
    root = y;
  }

  // Then delete key
  return delete_key(root, k);
}

bool Bstartree::delete_key(BsNode *x, unsigned long k) {
  // 0. find a key or link-position
  short i = find_left_most_key_or_right_bound_in_node(x, k);
  if (x->is_leaf) {
    // 1. x is leaf node and there is a 'k', just delete it
    if (x->keys[i].key == k && i < x->key_cnt) { // key found
      for (unsigned short j = i + 1; j < x->key_cnt; j++) {
        x->keys[j - 1] = x->keys[j];
      }
      x->key_cnt--;
      return true;
    }
    return false;
  }
  nm->check_node(x->p[i]);
  nm->check_node(x->p[i + 1]);
  if (i < x->key_cnt && x->keys[i].key == k) { // key found
    if (x->p[i]->key_cnt > key_min) {
      // 2.a x has k and max_leaf_node_in_subtree has more than t-1 keys
      BsNode *max_leaf = max_leaf_node_in_subtree(x->p[i]);
      x->keys[i] = max_leaf->keys[max_leaf->key_cnt - 1];
      return delete_key(x->p[i], x->keys[i].key);
    } else if (x->p[i + 1]->key_cnt > key_min) {
      // 2.b x has k and min_leaf_node_in_subtre has more than t-1 keys
      BsNode *min_leaf = min_leaf_node_in_subtree(x->p[i + 1]);
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

    BsNode *a = x->p[i];
    nm->check_node(x->p[i - 1]);
    if (i != 0 && x->p[i - 1]->key_cnt > key_min) {
      // 3.a.left
      // move key from p[i-1] via parent node(x)
      BsNode *b = x->p[i - 1];
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
      BsNode *c = x->p[i + 1];
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

void Bstartree::update_metric() {
  // height
  BsNode *x = this->root;
  mc.height = 1;
  while (!x->is_leaf) {
    x = x->p[0];
    mc.height += 1;
  }

  // node count & filling rate
  x = this->root;
  tree_walk_for_metric(x);
}

void Bstartree::tree_walk_for_metric(BsNode *x) {
  if (x->is_leaf) {
    mc.leaf_node_cnt += 1;
    mc.leaf_node_keys_cnt += x->key_cnt;
    return;
  } else {
    mc.intermediate_node_cnt += 1;
    mc.intermediate_node_keys_cnt += x->key_cnt;
    for (short i = 0; i < x->key_cnt; i++) {
      tree_walk_for_metric(x->p[i]);
    }
    tree_walk_for_metric(x->p[x->key_cnt]);
  }
}

void Bstartree::tree_walk(BsNode *x, vector<Item> *v) {
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
