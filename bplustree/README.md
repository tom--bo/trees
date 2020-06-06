# B plus tree

Extension of B-tree based on the book "Algorithm Introduction"

## How to test

```
make mytest

# clang++ bplustree.cpp mytest.cpp -o mytest -Wall -Wextra
# ./mytest
```

## Benchmark sample

```
make mybench

# clang++ bench.cpp bplustree.cpp -isystem benchmark/include -Lbenchmark/build/src -lbenchmark -lpthread -o gbench
# ./gbench
```

