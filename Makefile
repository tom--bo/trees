all:
	echo "a"

mytest:
	clang++ mytest.cpp btree/btree.cpp bplustree/bplustree.cpp -o mytest -Wall -Wextra -Ibtree -Ibplustree -Icommon
	./mytest
	rm mytest

mybench:
	clang++ -Wall mybench.cpp btree.cpp -o mybench
	./mybench
	rm mybench
