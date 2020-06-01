all:
	echo "a"

test:
	clang++ btree.cpp btree_test.cpp -o gtest -lgtest_main -lgtest -lpthread
	./gtest
	rm gtest

mytest:
	clang++ btree.cpp mytest.cpp -o mytest -Wall -Wextra
	./mytest
	rm mytest

bench:
	clang++ bench.cpp btree.cpp -isystem benchmark/include -Lbenchmark/build/src -lbenchmark -lpthread -o gbench
	./gbench
	rm gbench

mybench:
	clang++ -Wall mybench.cpp btree.cpp -o mybench
	./mybench
	rm mybench
