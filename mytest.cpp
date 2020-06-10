#include <bits/stdc++.h>
#include <stdlib.h>
#include "btree.h"
#include "bplustree.h"
using namespace std;
using namespace std::chrono;

/*
void bench(short int t, int exp_cnt, unsigned long ope_cnt, int mod,
           int del_rate) {

  long total_time = 0;
  for (int i = 0; i < exp_cnt; i++) {
    // init
    Bplustree b(t);
    mt19937_64 mt(i);

    steady_clock::time_point start = steady_clock::now();

    // start benchmark
    for (unsigned long j = 0; j < ope_cnt; j++) {
      unsigned long d = mt() % mod;
      if (mt() % 100 >= del_rate) {
        b.insert(Item{d, j});
      } else {
        b.delete_key(d);
      }
    }

    steady_clock::time_point finish = steady_clock::now();
    nanoseconds spent_time = duration_cast<nanoseconds>(finish - start);
    long st = spent_time.count();
    total_time += st;

    // end
    // b.print_metrics();
    // delete b;
  }

  long ave_time = total_time / exp_cnt;
  printf("T: %4d, ope_cnt: %lu, mod: %d, del(%%): %d, Ave: %3ld.%06ld ms\n", t,
         ope_cnt, mod, del_rate, ave_time / 1000000, ave_time % 1000000);
}

int main() {
  vector<short int> t = {2, 4, 8, 16, 32, 64, 128};
  for (int i = 0; i < t.size(); i++) {
    bench(t[i], 5, 100000, 2000, 10);
  }

  for (int i = 0; i < t.size(); i++) {
    bench(t[i], 5, 500000, 1000, 30);
  }

  for (int i = 0; i < t.size(); i++) {
    bench(t[i], 5, 1000000, 5000, 40);
  }
}
*/

map<unsigned long, unsigned long> mp;

void mp_add(unsigned long k) { mp[k]++; }
void mp_delete(unsigned long k) {
  if (mp[k] != 0) {
    mp[k]--;
  }
}

template <class T>
void test(T b, string filename, int num) {
  ifstream in(filename);
  cin.rdbuf(in.rdbuf());
  short int flag;
  unsigned long data, i = 0;
  unsigned long data_max = 0;

  // Btree b = Btree(t);
  mp = map<unsigned long, unsigned long>();

  while (cin >> flag >> data) {
    if (data > data_max)
      data_max = data;
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

  // count function test
  for (unsigned long i = 0; i <= data_max; i++) {
    if (b.count(i) != mp[i]) {
      cout << "case " << num << " failed: count(" << i << ") is different!"
           << endl;
      return;
    }
  }

  // data check
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
  // for (int n = 0; n < 4; n++) {
  for (int n = 0; n < 1; n++) {
    cout << "T = " << t[n] << " ------------- " << endl;
    // for (int i = 1; i <= 40; i++) {
    for (int i = 1; i <= 3; i++) {
      ios::fmtflags curret_flag = std::cout.flags();

      ostringstream ss;
      ss << std::setw(3) << std::setfill('0') << i;
      string s(ss.str());
      s = "tests/data/case_" + s + ".txt";
      std::cout.flags(curret_flag);

	  // Btree b = Btree(t[n]);
	  Bplustree b = Bplustree(t[n]);
      test(b, s, i);
    }
  }
  return 0;
}
