check: tests tests-*.conf
	./tests

tests: tests.cc inipp.hh
	g++ -I. -o $@ tests.cc
