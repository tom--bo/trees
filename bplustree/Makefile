all:
	echo "a"

test:
	clang++ bplustree.cpp bplustree_test.cpp -o gtest -lgtest_main -lgtest -lpthread -I../
	./gtest
	rm gtest

mytest:
	clang++ bplustree.cpp mytest.cpp -o mytest -Wall -Wextra -I../
	./mytest
	rm mytest

bench:
	clang++ bench.cpp bplustree.cpp -isystem benchmark/include -Lbenchmark/build/src -lbenchmark -lpthread -o gbench -I../
	./gbench
	rm gbench

mybench:
	clang++ -Wall mybench.cpp bplustree.cpp -o mybench -I../
	./mybench
	rm mybench
