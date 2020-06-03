#include "btree.h"
#include <bits/stdc++.h>
#include <gtest/gtest.h>
using namespace std;

void delete_vals(vector<Item> &v, unsigned long k) {
  unsigned long m, l = -1, r = v.size() - 1;
  while (r - l > 1) {
    m = (l + r) / 2;
    if (v[m].key >= k) {
      r = m;
    } else {
      l = m;
    }
  }
  v.erase(v.begin() + r);
}

void add_test_data(Btree &b, vector<Item> &v, vector<unsigned long> &keys) {
  unsigned long klen = keys.size();
  for (unsigned long i = 0; i < klen; i++) {
    Item item = Item{keys[i], i};
    b.insert(item);
    v.push_back(item);
  }
}

void del_test_data(Btree &b, vector<Item> &v, vector<unsigned long> &keys) {
  unsigned long klen = keys.size();
  for (unsigned long i = 0; i < klen; i++) {
    b.delete_key(keys[i]);
    delete_vals(v, keys[i]);
  }
}

void tree_walk_test(short t, vector<unsigned long> &add_keys,
                    vector<unsigned long> &del_keys) {
  vector<Item> v;
  Btree b = Btree(t);

  add_test_data(b, v, add_keys);

  // need to sort
  sort(v.begin(), v.end());

  del_test_data(b, v, del_keys);

  /* check by tree_walk() results;*/
  vector<Item> c;
  b.tree_walk(&c);

  unsigned long len = v.size();
  for (unsigned long i = 0; i < len; i++) {
    ASSERT_EQ(v[i].key, c[i].key);
  }
}

TEST(BtreeTest, only_add_t_2) {
  vector<unsigned long> add_keys = {10, 7,  8,  14, 12, 2, 4,  5,  3, 20, 21,
                                    17, 18, 16, 11, 1,  9, 13, 15, 6, 19};
  vector<unsigned long> del = {};

  tree_walk_test(2, add_keys, del);
}

TEST(BtreeTest, delete_1key_t_5) {
  vector<unsigned long> add_keys = {10, 7,  8,  14, 12, 2, 4,  5,  3, 20, 21,
                                    17, 18, 16, 11, 1,  9, 13, 15, 6, 19};
  vector<unsigned long> del = {1};
  tree_walk_test(5, add_keys, del);
}

TEST(BtreeTest, delete_keys_t_5_case_1) {
  vector<unsigned long> add_keys = {10, 7,  8,  14, 12, 2, 4,  5,  3, 20, 21,
                                    17, 18, 16, 11, 1,  9, 13, 15, 6, 19};
  vector<unsigned long> del = {1};
  tree_walk_test(5, add_keys, del);
}

TEST(BtreeTest, delete_keys_t_5_case_2) {
  vector<unsigned long> add_keys = {10, 7,  8,  14, 12, 2, 4,  5,  3, 20, 21,
                                    17, 18, 16, 11, 1,  9, 13, 15, 6, 19};
  vector<unsigned long> del = {1, 10};
  tree_walk_test(5, add_keys, del);
}

TEST(BtreeTest, delete_keys_t_5_case_3) {
  vector<unsigned long> add_keys = {10, 7,  8,  14, 12, 2, 4,  5,  3, 20, 21,
                                    17, 18, 16, 11, 1,  9, 13, 15, 6, 19};
  vector<unsigned long> del = {1, 2, 4, 8, 7, 9, 10, 13, 12, 19, 20, 3, 21};
  tree_walk_test(5, add_keys, del);
}

TEST(BtreeTest, delete_keys_t_5_case_4) {
  vector<unsigned long> add_keys = {10, 7,  8,  14, 12, 2, 4,  5,  3, 20, 21,
                                    17, 18, 16, 11, 1,  9, 13, 15, 6, 19};
  vector<unsigned long> del = {1, 2,  4, 8, 7,  9,  10, 13, 12, 19, 20,
                               3, 21, 5, 6, 11, 14, 15, 16, 17, 18};
  tree_walk_test(5, add_keys, del);
}

TEST(BtreeTest, duplicate_keys_t_5_case_1) {
  vector<unsigned long> add_keys = {11, 1, 1, 1, 1, 1, 9, 13, 15, 6, 19};
  vector<unsigned long> del = {1, 11, 13, 15, 19, 6, 1, 1};
  tree_walk_test(5, add_keys, del);
}

TEST(BtreeTest, duplicate_keys_t_5_case_2) {
  vector<unsigned long> add_keys = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                    1, 1, 1, 1, 1, 1, 1, 1, 1};
  vector<unsigned long> del = {1, 2};
  tree_walk_test(5, add_keys, del);
}

TEST(BtreeTest, duplicate_keys_t_5_case_3) {
  vector<unsigned long> add_keys = {
      31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,
      31, 3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,
      3,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2};
  vector<unsigned long> del = {1, 2, 3, 3, 3, 3, 3, 3, 3};
  tree_walk_test(5, add_keys, del);
}

// search() test
void search_test(short t, vector<unsigned long> &add_keys,
                 vector<unsigned long> &del_keys) {
  vector<Item> v;
  Btree b = Btree(t);

  add_test_data(b, v, add_keys);

  // need to sort
  sort(v.begin(), v.end());

  del_test_data(b, v, del_keys);

  unsigned long len = v.size();
  for (unsigned long i = 0; i < len; i++) {
    Item ret = b.search(v[i].key);
    ASSERT_EQ(ret.key, v[i].key);
  }
}

TEST(BtreeSearchTest, case1) {
  vector<unsigned long> add_keys = {1, 2, 3};
  vector<unsigned long> del_keys = {1};
  search_test(5, add_keys, del_keys);
}

TEST(BtreeSearchTest, case2) {
  vector<unsigned long> add_keys = {3, 1, 1, 1, 1, 2, 1, 2, 1,
                                    1, 2, 1, 2, 1, 2, 1, 2, 3};
  vector<unsigned long> del_keys = {1, 3};
  search_test(5, add_keys, del_keys);
}

TEST(BtreeSearchTest, case3) {
  vector<unsigned long> add_keys = {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 1,
                                    2,  3,  20, 20, 20, 20, 20, 20, 20, 20, 20,
                                    20, 20, 20, 20, 50, 50, 50, 50, 50, 50, 50};
  vector<unsigned long> del_keys = {1, 50, 50, 50, 50, 50, 20};
  search_test(5, add_keys, del_keys);
}

TEST(BtreeSearchTest, case4) {
  vector<unsigned long> add_keys = {
      1, 2, 3, 4, 5, 2, 3, 4, 5, 2, 3, 4, 5, 2, 3, 4, 5, 2, 3, 4,
      5, 2, 3, 4, 5, 2, 3, 4, 5, 2, 3, 4, 5, 2, 3, 4, 5, 2, 3, 4,
      5, 2, 3, 4, 5, 2, 3, 4, 5, 2, 3, 4, 5, 2, 3, 4, 5, 2, 3, 4,
      5, 2, 3, 4, 5, 2, 3, 4, 5, 2, 3, 4, 5, 2, 3, 4, 5};
  vector<unsigned long> del_keys = {1, 2, 3, 3};
  search_test(5, add_keys, del_keys);
}

TEST(BtreeSearchTest, case5) {
  vector<unsigned long> add_keys = {
      1, 2, 3, 4, 5, 2, 3, 4, 5, 2, 3, 4, 5, 2, 3, 4, 5, 2, 3, 4,
      5, 2, 3, 4, 5, 2, 3, 4, 5, 2, 3, 4, 5, 2, 3, 4, 5, 2, 3, 4,
      5, 2, 3, 4, 5, 2, 3, 4, 5, 2, 3, 4, 5, 2, 3, 4, 5, 2, 3, 4,
      5, 2, 3, 4, 5, 2, 3, 4, 5, 2, 3, 4, 5, 2, 3, 4, 5};
  vector<unsigned long> del_keys = {
      1, 2, 3, 4, 5, 2, 3, 4, 5, 2, 3, 4, 5, 2, 3, 4, 5, 2, 3, 4, 5, 2, 3, 4, 5,
      2, 3, 4, 5, 2, 3, 4, 5, 2, 3, 4, 5, 2, 3, 4, 5, 2, 3, 4, 5, 2, 3, 4, 5, 2,
      3, 4, 5, 2, 3, 4, 5, 2, 3, 4, 5, 2, 3, 4, 5, 2, 3, 4, 5, 2, 3, 4, 5, 2};
  search_test(5, add_keys, del_keys);
}
