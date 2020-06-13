#include "bastertree.h"
#include "bplustree.h"
#include "btree.h"
#include <bits/stdc++.h>
#include <stdlib.h>
#include <unistd.h>
using namespace std;
using namespace std::chrono;

template <class T> class TreeKit {
public:
  void bench(short int t, int exp_cnt, unsigned long ope_cnt, int mod,
             unsigned long del_rate) {
    long total_time = 0;
    for (int i = 0; i < exp_cnt; i++) {
      // init
      T tree(t);
      mt19937_64 mt(i);

      steady_clock::time_point start = steady_clock::now();

      // start benchmark
      for (unsigned long j = 0; j < ope_cnt; j++) {
        unsigned long d = mt() % mod;
        if (mt() % 100 >= del_rate) {
          tree.insert(Item{d, j});
        } else {
          tree.delete_key(d);
        }
      }

      steady_clock::time_point finish = steady_clock::now();
      nanoseconds spent_time = duration_cast<nanoseconds>(finish - start);
      long st = spent_time.count();
      total_time += st;

      // end
      // tree.print_metrics();
      // delete tree;
    }

    long ave_time = total_time / exp_cnt;
    printf("T: %4d, ope_cnt: %lu, mod: %d, del(%%): %lu, Ave: %3ld.%06ld ms\n",
           t, ope_cnt, mod, del_rate, ave_time / 1000000, ave_time % 1000000);
  }

  void test(string filename, short int t, int num) {
    ifstream in(filename);
    cin.rdbuf(in.rdbuf());
    short int flag;
    unsigned long data, i = 0;
    unsigned long data_max = 0;

    T tr(t);
    auto mp = map<unsigned long, unsigned long>();

    while (cin >> flag >> data) {
      if (data > data_max)
        data_max = data;
      i++;
      if (flag == 1) { // 1: add
        Item item = Item{data, i};
        tr.insert(item);
        mp[data]++;
      } else { // 2: delete
        tr.delete_key(data);
        if (mp[data] != 0) {
          mp[data]--;
        }
      }
    }

    // check
    vector<Item> c;
    tr.tree_walk(&c);

    // count function test
    for (unsigned long i = 0; i <= data_max; i++) {
      if (tr.count(i) != mp[i]) {
        cout << "case " << num << " failed: count(" << i << ") is different!"
             << endl;
        return;
      }
    }

    // count range test
    for (unsigned long i = 0; i < 10; i++) {
      for (unsigned long j = 30; j < 200; j += 5) {
        unsigned long tr_cnt = tr.count_range(i, j);
        // count in map
        unsigned long mp_cnt = 0;
        for (unsigned long k = i; k <= j; k++) {
          mp_cnt += mp[k];
        }
        if (tr_cnt != mp_cnt) {
          cout << "tree_cnt: " << tr_cnt << " -- map_cnt: " << mp_cnt << endl;
          return;
        }
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
};

string getTestFilename(int n) {
  ios::fmtflags curret_flag = std::cout.flags();

  ostringstream ss;
  ss << std::setw(3) << std::setfill('0') << n;
  string s(ss.str());
  s = "tests/data/case_" + s + ".txt";
  std::cout.flags(curret_flag);
  return s;
}

int main(int argc, char *argv[]) {
  // get options and args
  int opt;
  bool bench = false, test = false;
  while ((opt = getopt(argc, argv, "tbh")) != -1) {
    switch (opt) {
    case 'b':
      bench = true;
      break;
    case 't':
      test = true;
      break;

    case 'h':
      cout << "h: help" << endl;
      cout << "t: Execute my test" << endl;
      cout << "b: Execute my benchmark" << endl;
      break;

    default: /* '?' */
      cout << "Plz see help by -h" << endl;
      break;
    }
  }

  string tree = argv[optind];
  if (tree == "b")
    cout << "<< B tree >>" << endl;
  else if (tree == "bp")
    cout << "<< B+ tree >>" << endl;
  else if (tree == "ba")
    cout << "<< B* tree >>" << endl;

  /*
   *  TEST
   */
  if (test) {
    cout << "=== TEST START ===" << endl;
    vector<short> t = {2, 5, 10, 100};

    // t (threshold)
    for (int n = 0; n < 4; n++) {
      cout << "T = " << t[n] << " ------------- " << endl;

      // test case
      for (int i = 0; i <= 40; i++) {
        string s = getTestFilename(i);

        if (tree == "b") {
          auto tr = TreeKit<Btree>();
          tr.test(s, t[n], i);
        } else if (tree == "bp") {
          auto tr = TreeKit<Bplustree>();
          tr.test(s, t[n], i);
        } else if (tree == "ba") {
          auto tr = TreeKit<Bastertree>();
          tr.test(s, t[n], i);
        }
      }
    }
  }

  /*
   * benchmark
   */
  if (bench) {
    cout << "=== BENCH START ===" << endl;

    vector<short int> t = {2, 4, 8, 16, 32, 64, 128};
    if (tree == "b") {
      auto tr = TreeKit<Btree>();
      for (unsigned long i = 0; i < t.size(); i++) {
        tr.bench(t[i], 5, 100000, 2000, 10);
      }
      for (unsigned long i = 0; i < t.size(); i++) {
        tr.bench(t[i], 5, 500000, 1000, 30);
      }
      for (unsigned long i = 0; i < t.size(); i++) {
        tr.bench(t[i], 5, 1000000, 5000, 40);
      }
    } else if (tree == "bp") {
      auto tr = TreeKit<Bplustree>();
      for (unsigned long i = 0; i < t.size(); i++) {
        tr.bench(t[i], 5, 100000, 2000, 10);
      }
      for (unsigned long i = 0; i < t.size(); i++) {
        tr.bench(t[i], 5, 500000, 1000, 30);
      }
      for (unsigned long i = 0; i < t.size(); i++) {
        tr.bench(t[i], 5, 1000000, 5000, 40);
      }

    } else if (tree == "ba") {
      auto tr = TreeKit<Bastertree>();
      for (unsigned long i = 0; i < t.size(); i++) {
        tr.bench(t[i], 5, 100000, 2000, 10);
      }
      for (unsigned long i = 0; i < t.size(); i++) {
        tr.bench(t[i], 5, 500000, 1000, 30);
      }
      for (unsigned long i = 0; i < t.size(); i++) {
        tr.bench(t[i], 5, 1000000, 5000, 40);
      }
    }
  }

  return 0;
}
