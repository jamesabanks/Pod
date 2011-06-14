CFLAGS = -O2
LDFLAGS = -L../../ElectricFence-2.2.0 -L.. -lefence -lpod
LESS = /Users/james/Applications/MacVim.app/Contents/Resources/vim/runtime/macros/less.sh
tests = test_pod_char test_pod_object test_pod_string



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
