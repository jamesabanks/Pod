##################################################################### 
#   Makefile
#   Copyright (c) 2011-2012, James A. Banks
#   All rights reserved.
#   See file LICENSE for details.
##################################################################### 

CC=gcc
CFLAGS = -g -I../libpod
# LDFLAGS = -L.. -lpod -L../../ElectricFence-2.2.0 -lefence
LDFLAGS = -L.. -lpod -lefence
# LESS = /Users/james/Applications/MacVim.app/Contents/Resources/vim/runtime/macros/less.sh
LESS = /usr/share/vim/vim73/macros/less.sh
tests = test_pod_char test_pod_object test_pod_string test_pod_list \
        test_pod_mapping



.PHONY:	all
all:	$(tests) test ../libpod.a

test:	test.c
	$(CC) $(CFLAGS) test.c -o test

test_pod_char:	test_pod_char.c ../libpod.a
	$(CC) $(CFLAGS) test_pod_char.c -o test_pod_char $(LDFLAGS)

test_pod_object:	test_pod_object.c ../libpod.a
	$(CC) $(CFLAGS) test_pod_object.c -o test_pod_object $(LDFLAGS)

test_pod_string:	test_pod_string.c ../libpod.a
	$(CC) $(CFLAGS) test_pod_string.c -o test_pod_string $(LDFLAGS)

test_pod_list:	test_pod_list.c ../libpod.a
	$(CC) $(CFLAGS) test_pod_list.c -o test_pod_list $(LDFLAGS)

test_pod_mapping:	test_pod_mapping.c ../libpod.a
	$(CC) $(CFLAGS) test_pod_mapping.c -o test_pod_mapping $(LDFLAGS)

.PHONY: results
results:	all
	./test > test.results 2>&1
	$(LESS) test.results

.PHONY:	clean
clean:
	rm -f *.o $(tests) test test.results



# vim: noet ts=8
