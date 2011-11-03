CFLAGS = -O2 -Wall
sources_c = pod_char.c pod_string.c pod_list.c pod_mapping.c           \
    pod_map.c pod_scan_add_char.c pod_scan_add_token.c                 \
    pod_stream.c pod_stream_read.c pod_stream_write.c pod_scan_start.c \
    pod_scan_simple.c pod_scan_quoted.c pod_scan_escape.c              \
    pod_scan_escape_hex.c pod_scan_blurb.c pod_scan_append_to_buffer.c
sources_h = pod_blurb.h pod_boolean.h pod_char.h pod_list.h pod_log.h  \
    pod_mapping.h pod_marker.h pod_node.h pod_object.h pod_scan.h      \
    pod_scan_state.h pod_scan_token.h pod_stream.h pod_string.h
objects = $(sources_c:.c=.o)



.PHONY:	all
all:		depend libpod.a

.PHONY:	debug
debug:		CFLAGS := -g -Wall
debug:		libpod.a

libpod.a:	$(objects)
	rm -f libpod.a
	ar -crs libpod.a $(objects)

.PHONY: clean
clean:
	rm -f *.o depend

.PHONY: spotless
spotless:	clean
	rm -f libpod.a 

depend:	$(sources_c) $(sources_h)
	@$(CC) -MM $(sources_c) >> depend

-include depend


# vim: noet ts=8
