CFLAGS = -O2
LDFLAGS = -L.. -lpod
LESS = /Users/james/Applications/MacVim.app/Contents/Resources/vim/runtime/macros/less.sh
tests = test_pod_char test_pod_object



.PHONY:	all
all:	$(tests) test

test:	test.c
	$(CC) $(CFLAGS) test.c -o test

.PHONY: results
results:	all
	./test > test.results
	$(LESS) test.results

.PHONY:	clean
clean:
	rm -f *.o $(tests) test test.results



# vim: noet ts=8
