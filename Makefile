CFLAGS = -O2 -Wall
objects = pod_char.o pod_string.o pod_list.o pod_mapping.o  \
    pod_map.o pod_scan_add_char.o pod_scan_add_token.o \
    pod_stream.o pod_stream_read.o pod_stream_write.o pod_scan_start.o \
    pod_scan_simple.o pod_scan_quoted.o scan_escape.o scan_escape_hex.o scan_blurb.o



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
