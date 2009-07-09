
check: tests
	./tests

tests: tests.cc
	g++ -I. -o $@ $<
