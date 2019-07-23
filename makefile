def:
	gcc -Wall -g -fprofile-arcs -ftest-coverage -c -o json.o json.c
	gcc -Wall -g -fprofile-arcs -ftest-coverage -c -o demo.o demo.c
	gcc -Wall -g -fprofile-arcs -ftest-coverage -c -o xtest.o xtest.c
	gcc -Wall -o demo xtest.o demo.o json.o -lgcov

clean: 
	rm -f *.o *.gcda *.gcno *.gcov demo.info
	rm -rf demo_web
	rm -f demo

test: def
	./demo

lcov:
	lcov -d ./ -t 'demo' -o 'demo.info' -b . -c
	genhtml -o demo_web demo.info

.PHONY: def clean ut test
