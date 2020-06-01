# B tree

Refer the book "Algorithm Introduction"

## How to test

```
make mytest

# clang++ btree.cpp mytest.cpp -o mytest -Wall -Wextra
# ./mytest
```

## Benchmark sample

```
make mybench

# clang++ bench.cpp btree.cpp -isystem benchmark/include -Lbenchmark/build/src -lbenchmark -lpthread -o gbench
# ./gbench
```

