CC=gcc
CFLAGS = -g
# LDFLAGS = -L.. -lpod -L../../ElectricFence-2.2.0 -lefence
LDFLAGS = -L.. -lpod -lefence
# LESS = /Users/james/Applications/MacVim.app/Contents/Resources/vim/runtime/macros/less.sh
LESS = /usr/share/vim/vim73/macros/less.sh
tests = test_pod_char test_pod_object test_pod_string



.PHONY:	all
all:	$(tests) test ../libpod.a

test:	test.c
	$(CC) $(CFLAGS) test.c -o test

test_pod_char:	test_pod_char.c ../libpod.a
	$(CC) $(CFLAGS) test_pod_char.c -o test_pod_char $(LDFLAGS)

test_pod_object:	test_pod_object.c ../libpod.a
	$(CC) $(CFLAGS) $(LDFLAGS) test_pod_object.c -o test_pod_object $(LDFLAGS)

test_pod_string:	test_pod_string.c ../libpod.a
	$(CC) $(CFLAGS) $(LDFLAGS) test_pod_string.c -o test_pod_string $(LDFLAGS)

.PHONY: results
results:	all
	./test > test.results 2>&1
	$(LESS) test.results

.PHONY:	clean
clean:
	rm -f *.o $(tests) test test.results



# vim: noet ts=8
