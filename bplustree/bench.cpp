#include "bplustree.h"
#include <benchmark/benchmark.h>
#include <bits/stdc++.h>
using namespace std;

static void BM_GenRandomValue(benchmark::State &state) {
  ofstream ofs;
  ofs.open("test.txt", ios::out);

  mt19937 mt(0);
  long a = 0;
  for (auto _ : state) {
    a += mt();
    a %= 1000;
  }
  ofs.close();
}
// BENCHMARK(BM_GenRandomValue)->Complexity(benchmark::oN);

static void BM_WriteRandomToFile(benchmark::State &state) {
  ofstream ofs;
  ofs.open("test.txt", ios::out);

  mt19937 mt(state.range(0));
  for (auto _ : state) {
    ofs << mt() % 100 << endl;
  }
  ofs.close();
}
// BENCHMARK(BM_WriteRandomToFile)->Arg(1);

static void BM_BplustreeOnlyAdd(benchmark::State &state) {
  Bplustree b(state.range(0));

  mt19937 mt(0);
  for (auto _ : state)
    b.insert(Item{mt() % 1000, 0});
}
BENCHMARK(BM_BplustreeOnlyAdd)->RangeMultiplier(2)->Range(2, 128);

BENCHMARK_MAIN();
