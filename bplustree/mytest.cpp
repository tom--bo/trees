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

void test(short t, string filename, int num) {
  ifstream in(filename);
  cin.rdbuf(in.rdbuf());
  short int flag;
  unsigned long data, i = 0;
  unsigned long data_max = 0;

  Bplustree b = Bplustree(t);
  mp = map<unsigned long, int>();

  while (cin >> flag >> data) {
    i++;
    if (data > data_max)
      data_max = data;
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

  // count function test
  for (unsigned long i = 0; i <= data_max; i++) {
    unsigned long bcnt = b.count(i);
    if (bcnt != mp[i]) {
      cout << "case " << num << " failed: count(" << i << ") is different!"
           << endl;
      cout << "bcnt: " << bcnt << "-- map: " << mp[i] << endl;
      return;
    }
  }

  // data check
  int diff_from = -1;
  int l = c.size();
  //? cout << "size: " <<  p.size() << endl;
  for (int i = 0; i < l; i++) {
    //? cout << p[i].key << endl;
    if (diff_from == -1 && mp[c[i].key] <= 0) {
      diff_from = i;
      break;
    }
    mp[c[i].key]--;
  }

  // cout << "========================================"  << endl;

  for (auto i = mp.begin(); i != mp.end(); ++i) {
    if (i->second != 0) {
      // cout << i->first << ": " << i->second << endl;
      // cout << "case " << num << " failed: remaining key in map test" << endl;
      // return;
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
    for (int i = 0; i <= 40; i++) {
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
