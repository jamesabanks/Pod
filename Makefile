objects = pod_object.o pod_string.o

all:	libpod.a

libpod.a:	$(objects)

.PHONY: clean
clean:
	rm -f *.o libpod.a



# vim: noet
