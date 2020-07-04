all:
	echo "a"

build:
	clang++ -O2 main.cpp common/tree_common.cpp btree/btree.cpp bplustree/bplustree.cpp bstartree/bstartree.cpp -o main -Wall -Wextra -Ibtree -Ibplustree -Ibstartree -Icommon


