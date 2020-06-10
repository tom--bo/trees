all:
	echo "a"

test:
	clang++ btree.cpp btree_test.cpp -o gtest -lgtest_main -lgtest -lpthread -I../
	./gtest
	rm gtest

mytest:
	clang++ btree.cpp mytest.cpp -o mytest -Wall -Wextra -I../
	./mytest
	rm mytest

bench:
	clang++ bench.cpp btree.cpp -isystem benchmark/include -Lbenchmark/build/src -lbenchmark -lpthread -o gbench -I../
	./gbench
	rm gbench

mybench:
	clang++ -Wall mybench.cpp btree.cpp -o mybench -I../
	./mybench
	rm mybench
