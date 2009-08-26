check: tests tests-*.conf
	./tests

tests: tests.cc inipp.hh
	g++ -Wall -Werror -I. -o $@ tests.cc
