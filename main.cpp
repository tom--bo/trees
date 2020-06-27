#include "bplustree.h"
#include "bstartree.h"
#include "btree.h"
#include <bits/stdc++.h>
#include <unistd.h>
using namespace std;
using namespace std::chrono;

string getTestFilename(int n) {
  ios::fmtflags curret_flag = std::cout.flags();

  ostringstream ss;
  ss << std::setw(3) << std::setfill('0') << n;
  string s(ss.str());
  s = "tests/data/case_" + s + ".txt";
  std::cout.flags(curret_flag);
  return s;
}

string getSimpleTestFilename(int n) {
  ios::fmtflags curret_flag = std::cout.flags();

  ostringstream ss;
  ss << std::setw(3) << std::setfill('0') << n;
  string s(ss.str());
  s = "tests/simple_data/case_" + s + ".txt";
  std::cout.flags(curret_flag);
  return s;
}

template <class T> class TestManager {
public:
  void exec_benchmark(bool debug) {
    cout << "=== BENCH START ===" << endl;
    vector<int> t = {4, 16, 64, 256};
    bench(debug, t, 10, 1000000, 10000, 0);
    bench(debug, t, 10, 1000000, 10000, 10);
    bench(debug, t, 10, 1000000, 10000, 20);
  }

  void exec_test(bool debug) {
    cout << "=== TEST START ===" << endl;
    vector<short> t = {2, 5, 10, 100};
    for (unsigned int n = 0; n < t.size(); n++) {
      int ng = 0;
      cout << "T = " << t[n] << " ------------- " << endl;
      for (int i = 0; i <= 40; i++) {
        string s = getTestFilename(i);
        ng += test(debug, s, t[n], i);
      }
      if (ng == 0) {
        cout << "All tests passed!!" << endl;
      }
    }
  }

  void exec_simple_test(bool debug) {
    int ng = 0;
    for (int i = 0; i <= 10; i++) {
      string s = getSimpleTestFilename(i);
      ng += test(debug, s, 2, i);
    }
    if (ng == 0) {
      cout << "All tests passed!!" << endl;
    }
  }

private:
  void bench(bool debug, vector<int> vt, int exp_cnt, unsigned long ope_cnt, int mod,
             unsigned long del_rate) {
    printf("\nope_cnt: %lu, mod: %d, del(%%): %lu\n", ope_cnt, mod, del_rate);
    cout << "  T, Ave_total_time(ms)";
    if (debug) {
      cout << ", Ave_node_cnt, Ave_node_merge, Ave_node_split";
    }
    cout << endl;

    // vector<short int> vt = {2, 4, 8, 16, 32, 64, 128};
    for (unsigned int i = 0; i < vt.size(); i++) {

      long total_time = 0;
      MetricCounter total_mc = MetricCounter();
      for (int j = 0; j < exp_cnt; j++) {
        // init
        T tree(vt[i]);
        mt19937_64 mt(j);

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
        MetricCounter mc = tree.get_metrics();
        total_mc.node_count += mc.node_count;
        total_mc.node_merge += mc.node_merge;
        total_mc.node_split += mc.node_split;

        // end
        // delete tree;
      }

      long ave_time = total_time / exp_cnt;
      /*
      printf(
          "T: %4d, ope_cnt: %lu, mod: %d, del(%%): %lu, Ave: %3ld.%06ld ms\n",
          vt[i], ope_cnt, mod, del_rate, ave_time / 1000000,
          ave_time % 1000000);
      */
      printf("%3d, %11ld.%06ld", vt[i], ave_time / 1000000, ave_time % 1000000);
      if (debug) {
        /*
        printf(
            "Ave_Node_cnt: %6lu, Ave_Node_merge: %6lu, Ave_Node_split: %6lu\n",
            total_mc.node_count / exp_cnt, total_mc.node_merge / exp_cnt,
            total_mc.node_split / exp_cnt);
        */
        printf(", %12lu, %14lu, %14lu", total_mc.node_count / exp_cnt,
               total_mc.node_merge / exp_cnt, total_mc.node_split / exp_cnt);
      }
      cout << endl;
    }
  }

  int test(bool debug, string filename, short int t, int num) {
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
        cout << "expected: " << mp[i] << ", returned: " << tr.count(i) << endl;
        return 1;
      }
    }
    // search test
    for (unsigned long i = 1; i <= data_max; i++) {
      Item a = tr.search(i);
      if ((mp[i] > 0 && a.key == 0) || (mp[i] == 0 && a.key > 0)) {
        cout << "case " << num << " failed: search(" << i << ") is different!"
             << endl;
        cout << "expected: " << (mp[i] != 0) << ", returned: " << (tr.search(i).key != 0) << endl;
        return 1;
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
          return 1;
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
        return 1;
      }
    }

    if (diff_from == -1) {
      if (debug) {
        cout << "case " << num << " passed!" << endl;
      }
    } else {
      cout << "case " << num << " failed: ";
      cout << "different data from " << diff_from << "  --  " << endl;
    }
    return 0;
  }
};

int main(int argc, char *argv[]) {
  // get options and args
  int opt;
  bool bench = false, test = false, simple_test = false;
  bool debug = false;
  while ((opt = getopt(argc, argv, "bdhst")) != -1) {
    switch (opt) {
    case 'b':
      bench = true;
      break;
    case 's':
      simple_test = true;
      break;
    case 't':
      test = true;
      break;
    case 'd':
      debug = true;
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

  if (tree == "b") {
    cout << "<< B tree >>" << endl;
    auto tm = TestManager<Btree>();
    if (simple_test)
      tm.exec_simple_test(debug);
    if (test)
      tm.exec_test(debug);
    if (bench)
      tm.exec_benchmark(debug);
  } else if (tree == "bp") {
    cout << "<< B+ tree >>" << endl;
    auto tm = TestManager<Bplustree>();
    if (simple_test)
      tm.exec_simple_test(debug);
    if (test)
      tm.exec_test(debug);
    if (bench)
      tm.exec_benchmark(debug);
  } else if (tree == "bs") {
    cout << "<< B* tree >>" << endl;
    auto tm = TestManager<Bstartree>();
    if (simple_test)
      tm.exec_simple_test(debug);
    if (test)
      tm.exec_test(debug);
    if (bench)
      tm.exec_benchmark(debug);
  }

  return 0;
}
