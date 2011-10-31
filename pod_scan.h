#ifndef INCLUDE_POD_SCAN_H
#define INCLUDE_POD_SCAN_H

#include "pod_char.h"
#include "pod_stream.h"



extern int pod_scan_start(pod_stream *stream, pod_char_t c);
extern int pod_scan_escape(pod_stream *stream, pod_char_t c);
extern int pod_scan_escape_hex(pod_stream *stream, pod_char_t c);
extern int pod_scan_simple(pod_stream *stream, pod_char_t c);
extern int pod_scan_quoted(pod_stream *stream, pod_char_t c);
extern int pod_scan_blurb(pod_stream *stream, pod_char_t c);
extern int pod_scan_blurb_pre_size(pod_stream *stream, pod_char_t c);
extern int pod_scan_blurb_size(pod_stream *stream, pod_char_t c);
extern int pod_scan_blurb_post_size(pod_stream *stream, pod_char_t c);




#endif /* INCLUDE_POD_SCAN_H */
