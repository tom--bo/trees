#include "bplustree.h"
#include <bits/stdc++.h>
using namespace std;

map<unsigned long, int> mp;

void mp_add(unsigned long k) { mp[k]++; }
void mp_delete(unsigned long k) {
  if (mp[k] != 0) {
    mp[k]--;
  }
}

int main() {
  unsigned short t = 2;
  // vector<unsigned long> d = {1, 2, 3, 4, 5};
  vector<unsigned long> d = {10, 7,  8,  14, 12, 2, 4,  5,  3, 20, 21,
                             17, 18, 16, 11, 1,  9, 13, 15, 6, 19};
  Bplustree b = Bplustree(t);
  mp = map<unsigned long, int>();

  unsigned long data_max = 0;

  for (unsigned int i = 0; i < d.size(); i++) {
    if (d[i] > data_max) {
      data_max = d[i];
    }
    Item item = Item{d[i], i};
    b.insert(item);
    mp_add(d[i]);
  }

  // check
  vector<Item> c;
  b.tree_walk(&c);

  //// count function test
  // for (unsigned long i = 0; i <= data_max; i++) {
  //  if (b.count(i) != mp[i]) {
  //    cout << "Failed: count(" << i << ") is different!" << endl;
  //    break;
  //  }
  //}

  // data check
  int diff_from = -1;
  int l = c.size();
  unsigned max_val = 0;
  for (int i = 0; i < l; i++) {
    if(max_val < c[i].key) {
      max_val = c[i].key;
    } else if(max_val > c[i].key){
      cout << "dumped vector's order is wired!" << endl;
      return 0;
    }
    if (diff_from == -1 && mp[c[i].key] <= 0) {
      diff_from = i;
      break;
    }
    mp[c[i].key]--;
  }

  if (diff_from == -1) {
    cout << "Succeeded!" << endl;
  } else {
    cout << "Different data from " << diff_from << "  --  " << endl;
    return 0;
  }

  for (auto i = mp.begin(); i != mp.end(); ++i) {
    if (i->second != 0) {
      cout << "Failed: remaining key in map test. i: " << i->first << endl;
      return 0;
    }
  }

  return 0;
}
