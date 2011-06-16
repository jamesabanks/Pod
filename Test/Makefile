# TODO This makefile needs work.  Each test needs its own dependency list, and
# its own LDFLAGS variable.

# TODO Make TODOs in Makefiles highlighted like in C files.

# TODO Make plain strings with 1 indent (4 spaces) stand out more in results
# files.

CFLAGS = -g
LDFLAGS = -L../../ElectricFence-2.2.0 -L.. -lefence -lpod
LESS = /Users/james/Applications/MacVim.app/Contents/Resources/vim/runtime/macros/less.sh
tests = test_pod_char test_pod_object test_pod_string



.PHONY:	all
all:	$(tests) test ../libpod.a

test:	test.c
	$(CC) $(CFLAGS) test.c -o test

.PHONY: results
results:	all
	./test > test.results 2>&1
	$(LESS) test.results

.PHONY:	clean
clean:
	rm -f *.o $(tests) test test.results



# vim: noet ts=8
