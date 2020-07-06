# What is this

A collection of tree data structure to learn.

# How to build

Use Makefile. (CMake is prepared only for IDE)

```
make build
```

# How to run

## Test

```
make build
./main -t b
```

- `-t` means `run test` or `-s` (simple test)
- `b (argument)` means `b tree`. Other candidates are `bp` (b-plus) and `bs` (b-star).

## Benchmark

```
make build
./main -b -d bp
```

- `-d` means debug (print additional metrics)



# Implementation List

- Red Black Tree
- AVL Tree
- [x] B Tree : Refer the book "Algorithm Introduction"
- [x] B+ Tree : Extend B tree by myself
- [x] B\* Tree
- [ ] B-Link Tree
- [ ] LSM Tree
- [ ] Skip List


# License

This software is released under the MIT License, see LICENSE.txt.
