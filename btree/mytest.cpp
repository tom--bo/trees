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

map<unsigned long, int> mp;

void mp_add(unsigned long k) { mp[k]++; }
void mp_delete(unsigned long k) {
  if (mp[k] != 0) {
    mp[k]--;
  }
}

void test(short t, string filename, int num) {
  ifstream in(filename);
  cin.rdbuf(in.rdbuf());
  short int flag;
  unsigned long data, i = 0;

  Btree b = Btree(t);
  mp = map<unsigned long, int>();

  while (cin >> flag >> data) {
    i++;
    if (flag == 1) { // 1: add
      Item item = Item{data, i};
      b.insert(item);
      mp_add(data);
    } else { // 2: delete
      b.delete_key(data);
      mp_delete(data);
    }
  }

  // check
  vector<Item> c;
  b.tree_walk(&c);

  int diff_from = -1;
  int l = c.size();
  for (int i = 0; i < l; i++) {
    if (diff_from == -1 && mp[c[i].key] <= 0) {
      diff_from = i;
      break;
    }
    mp[c[i].key]--;
  }

  for (auto i = mp.begin(); i != mp.end(); ++i) {
    if (i->second != 0) {
      cout << "case " << num << " failed: remaining key in map test" << endl;
      return;
    }
  }

  if (diff_from == -1) {
    cout << "case " << num << " succeeded!" << endl;
  } else {
    cout << "case " << num << " failed: ";
    cout << "different data from " << diff_from << "  --  " << endl;
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
