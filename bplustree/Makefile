all:
	echo "a"

test:
	clang++ bplustree.cpp bplustree_test.cpp -o gtest -lgtest_main -lgtest -lpthread
	./gtest
	rm gtest

mytest:
	clang++ bplustree.cpp mytest.cpp -o mytest -Wall -Wextra
	./mytest
	rm mytest

bench:
	clang++ bench.cpp bplustree.cpp -isystem benchmark/include -Lbenchmark/build/src -lbenchmark -lpthread -o gbench
	./gbench
	rm gbench

mybench:
	clang++ -Wall mybench.cpp bplustree.cpp -o mybench
	./mybench
	rm mybench
