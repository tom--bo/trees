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

class TestManager {
public:
  IndexType index_type;

  TestManager(IndexType it) : index_type{it} {};

  void exec_benchmark(bool debug, unsigned int lru_capa) {
    cout << "=== BENCH START ===" << endl;
    vector<int> t = {/*4, 8, 16, 32,*/ 64, 128, 256, 512};
    bench(debug, lru_capa, t, 10, 100000, 1000, 100000, 0, 0, 100, 0);
    // bench(debug, lru_capa, t, 10, 1000000, 1000, 100000, 0, 0, 90, 10);
    // bench(debug, lru_capa, t, 10, 1000000, 1000, 100000, 0, 0, 80, 20);
  }

  void exec_test(bool debug, unsigned int lru_capa) {
    cout << "=== TEST START ===" << endl;
    vector<short> t = {2, 5, 10, 100};
    for (unsigned int n = 0; n < t.size(); n++) {
      int ng = 0;
      cout << "T = " << t[n] << " ------------- " << endl;
      for (int i = 0; i <= 40; i++) {
        string s = getTestFilename(i);
        ng += test(debug, lru_capa, s, t[n], i);
      }
      if (ng == 0) {
        cout << "All tests passed!!" << endl;
      }
    }
  }

  void exec_simple_test(bool debug, unsigned int lru_capa) {
    int ng = 0;
    for (int i = 0; i <= 10; i++) {
      string s = getSimpleTestFilename(i);
      ng += test(debug, lru_capa, s, 2, i);
    }
    if (ng == 0) {
      cout << "All tests passed!!" << endl;
    }
  }

private:
  Indexable *buildTree(IndexType it, short int t, unsigned int lru_capa) {
    switch (it) {
    case 0:
      return new Btree(t, lru_capa);
    case 1:
      return new Bplustree(t, lru_capa);
    case 2:
      return new Bstartree(t, lru_capa);
    }
  }

  void bench(bool debug, unsigned int lru_capa, vector<int> vt, int exp_cnt,
             unsigned long ope_cnt, unsigned int initial_insert, int mod,
             unsigned long select_pct, unsigned long range_pct,
             unsigned long insert_pct, unsigned long del_pct) {
    printf("\nope_cnt: %lu, mod: %d, lru_capa: %d, select(%%): %lu, range(%%): "
           "%lu, "
           "insert(%%): %lu, del(%%): %lu\n",
           ope_cnt, mod, lru_capa, select_pct, range_pct, insert_pct, del_pct);
    cout << "  T, Ave_total_time(ms)";
    if (debug) {
      cout << ", Ave_node_cnt, Ave_inter_node, Ave_leaf_node, Ave_root_keys, "
              "Ave_inter_fill_rate, Ave_leaf_fill_rate, Ave_node_merge, "
              "Ave_node_split, Ave_tree_height, Cache_hit_rate";
    }
    cout << endl;

    // vector<short int> vt = {2, 4, 8, 16, 32, 64, 128};
    for (unsigned int i = 0; i < vt.size(); i++) {
      long total_time = 0;
      MetricCounter total_mc = MetricCounter();
      unsigned key_max;
      for (int j = 0; j < exp_cnt; j++) {
        // init
        Indexable *tree = buildTree(index_type, vt[i], lru_capa);
        key_max = tree->get_key_max();
        mt19937_64 mt(j);

        steady_clock::time_point start = steady_clock::now();

        // start benchmark
        unsigned short select_ = select_pct;
        unsigned short range_ = select_ + range_pct;
        unsigned short insert_ = range_ + insert_pct;
        if (insert_ + del_pct != 100) {
          cout << "select, range, insert, del rate setting is invalid!" << endl;
          return;
        }
        // initial insert
        for (unsigned j = 0; j < initial_insert; j++) {
          unsigned long d = mt() % mod;
          tree->insert(Item{d, j});
        }

        // operations in experiment
        for (unsigned long j = 0; j < ope_cnt; j++) {
          unsigned long d1 = mt() % mod;
          unsigned long threshold = mt() % 100;
          if (threshold < select_) {
            tree->search(d1);
          } else if (threshold < range_) {
            unsigned long d2 = mt() % mod;
            if (d1 > d2)
              swap(d1, d2);
            tree->count_range(d1, d2);
          } else if (threshold < insert_) {
            tree->insert(Item{d1, j});
          } else {
            tree->delete_key(d1);
          }
        }

        steady_clock::time_point finish = steady_clock::now();
        nanoseconds spent_time = duration_cast<nanoseconds>(finish - start);
        long st = spent_time.count();
        total_time += st;
        tree->update_metric();
        MetricCounter mc = tree->get_metrics();
        total_mc.node_count += mc.node_count;
        total_mc.root_key_count += mc.root_key_count;
        total_mc.intermediate_node_cnt += mc.intermediate_node_cnt;
        total_mc.intermediate_node_keys_cnt += mc.intermediate_node_keys_cnt;
        total_mc.leaf_node_cnt += mc.leaf_node_cnt;
        total_mc.leaf_node_keys_cnt += mc.leaf_node_keys_cnt;
        total_mc.node_merge += mc.node_merge;
        total_mc.node_split += mc.node_split;
        total_mc.height += mc.height;
        total_mc.cache_checked += mc.cache_checked;
        total_mc.cache_hit_cnt += mc.cache_hit_cnt;
        // end
        // delete tree;
      }

      long ave_time = total_time / exp_cnt;
      printf("%3d, %11ld.%06ld", vt[i], ave_time / 1000000, ave_time % 1000000);
      if (debug) {
        if (total_mc.cache_checked == 0)
          total_mc.cache_checked = 1.0;
        printf(
            ", %12.1f, %14.1f, %13.1f, %13.1f, %19.1f, %18.1f, %14.1f, %18.1f, "
            "%15.1f, %10.1f",
            total_mc.node_count / exp_cnt,
            total_mc.intermediate_node_cnt / exp_cnt,
            total_mc.leaf_node_cnt / exp_cnt, total_mc.root_key_count / exp_cnt,
            total_mc.intermediate_node_keys_cnt /
                total_mc.intermediate_node_cnt / key_max * 100,
            total_mc.leaf_node_keys_cnt / total_mc.leaf_node_cnt / key_max *
                100,
            total_mc.node_merge / exp_cnt, total_mc.node_split / exp_cnt,
            total_mc.height / exp_cnt,
            total_mc.cache_hit_cnt / total_mc.cache_checked * 100);
      }
      cout << endl;
    }
  }

  int test(bool debug, unsigned int lru_capa, string filename, short int t,
           int num) {
    ifstream in(filename);
    cin.rdbuf(in.rdbuf());
    short int flag;
    unsigned long data, i = 0;
    unsigned long data_max = 0;

    Indexable *tr = buildTree(index_type, t, lru_capa);
    auto mp = map<unsigned long, unsigned long>();

    while (cin >> flag >> data) {
      if (data > data_max)
        data_max = data;
      i++;
      if (flag == 1) { // 1: add
        Item item = Item{data, i};
        tr->insert(item);
        mp[data]++;
      } else { // 2: delete
        tr->delete_key(data);
        if (mp[data] != 0) {
          mp[data]--;
        }
      }
    }

    // check
    vector<Item> c;
    tr->tree_walk(&c);

    // count function test
    for (unsigned long i = 0; i <= data_max; i++) {
      if (tr->count(i) != mp[i]) {
        cout << "case " << num << " failed: count(" << i << ") is different!"
             << endl;
        cout << "expected: " << mp[i] << ", returned: " << tr->count(i) << endl;
        return 1;
      }
    }
    // search test
    for (unsigned long i = 1; i <= data_max; i++) {
      Item a = tr->search(i);
      if ((mp[i] > 0 && a.key == 0) || (mp[i] == 0 && a.key > 0)) {
        cout << "case " << num << " failed: search(" << i << ") is different!"
             << endl;
        cout << "expected: " << (mp[i] != 0)
             << ", returned: " << (tr->search(i).key != 0) << endl;
        return 1;
      }
    }

    // count range test
    for (unsigned long i = 0; i < 10; i++) {
      for (unsigned long j = 30; j < 200; j += 5) {
        unsigned long tr_cnt = tr->count_range(i, j);
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
  unsigned int lru_capa = 0;
  while ((opt = getopt(argc, argv, "bdhl:st")) != -1) {
    switch (opt) {
    case 'b':
      bench = true;
      break;
    case 'd':
      debug = true;
      break;
    case 'l':
      lru_capa = stoi(string(optarg));
      break;
    case 'h':
      cout << "./main (-b|-t|-s) [-d] [-l (num)] (b|bp|bs) " << endl;
      cout << "-h: help" << endl;
      cout << "-t: Execute my test" << endl;
      cout << "-s: Execute my simple test" << endl;
      cout << "-b: Execute my benchmark" << endl;
      cout << "-d: (Debug mode) Print additional info" << endl;
      cout << "-l [num]: define LRU cache capacity (node number). If you don't "
              "specify this, LRU cache is disabled (Not assumes disk storage)"
           << endl;
      return 1;
    case 's':
      simple_test = true;
      break;
    case 't':
      test = true;
      break;

    default: /* '?' */
      cout << "Plz see help by -h" << endl;
      return 1;
    }
  }
  string tree = argv[optind];

  IndexType index_type = B;
  if (tree == "b") {
    index_type = B;
    cout << "<< B tree >>" << endl;
  } else if (tree == "bp") {
    index_type = B_PLUS;
    cout << "<< Bplus tree >>" << endl;
  } else if (tree == "bs") {
    index_type = B_STAR;
    cout << "<< Bstar tree >>" << endl;
  } else {
    cout << "No index type specified!!" << endl;
    return 1;
  }

  auto tm = TestManager(index_type);
  if (simple_test)
    tm.exec_simple_test(debug, lru_capa);
  if (test)
    tm.exec_test(debug, lru_capa);
  if (bench)
    tm.exec_benchmark(debug, lru_capa);

  return 0;
}
