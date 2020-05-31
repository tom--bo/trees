#include "btree.h"
#include <bits/stdc++.h>
#include <stdlib.h>
using namespace std;
using namespace std::chrono;

void bench(short int t, int exp_cnt, unsigned long ope_cnt, int mod,
           int del_rate) {

  long total_time = 0;
  for (int i = 0; i < exp_cnt; i++) {
    // init
    BTree b(t);
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
    // delete b;
  }

  long ave_time = total_time / exp_cnt;
  printf("T: %4d, ope_cnt: %lu, mod: %d, del(%%): %d, Ave: %3ld.%06ld ms\n", t, ope_cnt, mod,
         del_rate, ave_time / 1000000, ave_time % 1000000);
}

int main() {
  vector<short int> t = {2, 4, 8, 16, 32, 64, 128};
  for (int i = 0; i < t.size(); i++) {
    bench(t[i], 5, 100000, 2000, 10);
  }
}
