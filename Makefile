CFLAGS = -O2
objects = pod_char.o pod_object.o pod_string.o



.PHONY:	all
all:		libpod.a

.PHONY:	debug
debug:		CFLAGS := -g -Wall
debug:		$(objects) libpod.a

libpod.a:	$(objects)
	rm -f libpod.a
	ar -crs libpod.a $(objects)

.PHONY: clean
clean:
	rm -f *.o

.PHONY: spotless
spotless:	clean
	rm -f libpod.a



# vim: noet ts=8
