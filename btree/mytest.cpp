#include "btree.h"
#include <bits/stdc++.h>
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
  if (v[r].key == k) {
    v.erase(v.begin() + r);
  }
}

void test(short t, string filename, int num) {
  ifstream in(filename);
  cin.rdbuf(in.rdbuf());
  short int flag;
  unsigned long data, i = 0;

  Btree b = Btree(t);
  vector<Item> v;

  while (cin >> flag >> data) {
    i++;
    if (flag == 1) { // 1: add
      Item item = Item{data, i};
      b.insert(item);
      v.push_back(item);
    } else { // 2: delete
      b.delete_key(data);
      if (v.size() > 0) {
        sort(v.begin(), v.end());
        delete_vals(v, data);
      }
    }
  }
  sort(v.begin(), v.end());

  // check
  vector<Item> c;
  b.tree_walk(&c);

  // cout << " -- check -- " << endl;

  int diff_from = -1;
  int l = v.size();
  for (int i = 0; i < l; i++) {
    // printf("%3d: -- ", i);
    // printf("key: %2ld, val: %2ld -- ", v[i].key, v[i].val);
    // printf("key: %2ld, val: %2ld\n", c[i].key, c[i].val);
    if (diff_from == -1 && v[i].key != c[i].key) {
      // cout << "=========" << v[i].key << " " << c[i].key << endl;
      diff_from = i;
    }
  }

  if (diff_from == -1) {
    cout << "case " << num << " succeeded!" << endl;
  } else {
    cout << "case " << num << " failed: ";
    cout << "different data from " << diff_from << "  --  ";
  }
  return;
}

int main() {
  vector<short> t = {2, 5, 10, 100};
  for (int n = 0; n < 4; n++) {
    cout << "T = " << t[n] << " ------------- " << endl;
    for (int i = 1; i <= 40; i++) {
      ios::fmtflags curret_flag = std::cout.flags();

      ostringstream ss;
      ss << std::setw(3) << std::setfill('0') << i;
      string s(ss.str());
      s = "tests/data/case_" + s + ".txt";
      std::cout.flags(curret_flag);

      test(t[n], s, i);
    }
  }
  return 0;
}
