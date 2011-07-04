CFLAGS = -g
LDFLAGS = -L../../ElectricFence-2.2.0 -L.. -lefence -lpod
LESS = /Users/james/Applications/MacVim.app/Contents/Resources/vim/runtime/macros/less.sh
tests = test_pod_char test_pod_object test_pod_string



.PHONY:	all
all:	$(tests) test ../libpod.a

test:	test.c
	$(CC) $(CFLAGS) test.c -o test

test_pod_char:	test_pod_char.c ../libpod.a
	$(CC) $(CFLAGS) $(LDFLAGS) test_pod_char.c -o test_pod_char

test_pod_object:	test_pod_object.c ../libpod.a
	$(CC) $(CFLAGS) $(LDFLAGS) test_pod_object.c -o test_pod_object

test_pod_string:	test_pod_string.c ../libpod.a
	$(CC) $(CFLAGS) $(LDFLAGS) test_pod_string.c -o test_pod_string

.PHONY: results
results:	all
	./test > test.results 2>&1
	$(LESS) test.results

.PHONY:	clean
clean:
	rm -f *.o $(tests) test test.results



# vim: noet ts=8
