#include <bits/stdc++.h>
#include <stdlib.h>
using namespace std;

string make_filename(int i) {
  ios::fmtflags curret_flag = std::cout.flags();

  ostringstream os;
  os << std::setw(3) << std::setfill('0') << i;
  string f(os.str());
  f = "case_" + f + ".txt";
  std::cout.flags(curret_flag);
  return f;
}

void make_test_case(int start, int end, int mod, int del_rate, int cnt) {
  for (int i = start; i <= end; i++) {
    string f = make_filename(i);
    ofstream ofs;
    ofs.open(f, ios::out);

    mt19937 mt(i);
    for (int j = 0; j < cnt; j++) {
      // 1: add, 2, delete
      if (mt() % 100 >= del_rate) {
        ofs << 1 << " ";
      } else {
        ofs << 2 << " ";
      }
      ofs << mt() % mod << endl;
    }
    ofs.close();
  }
}

int main() {
  make_test_case(1, 10, 3000, 20, 300);
  make_test_case(11, 20, 1000, 10, 10000);
  make_test_case(21, 30, 500, 30, 10000);
  make_test_case(31, 40, 50, 40, 10000);
}
