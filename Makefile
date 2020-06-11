all:
	echo "a"

build:
	clang++ -O2 main.cpp btree/btree.cpp bplustree/bplustree.cpp -o main -Wall -Wextra -Ibtree -Ibplustree -Icommon


