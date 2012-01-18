#ifndef INCLUDE_POD_SCAN_H
#define INCLUDE_POD_SCAN_H

/******************************************************************** 
 *  pod_scan.h
 *  Copyright (c) 2011-2012, James A. Banks
 *  All rights reserved.
 *  See file LICENSE for details.
 ********************************************************************/

#include "pod_char.h"
#include "pod_stream.h"
#include "pod_string.h"



extern int pod_scan_start(pod_stream *stream, pod_char_t c);
extern int pod_scan_escape(pod_stream *stream, pod_char_t c);
extern int pod_scan_escape_hex(pod_stream *stream, pod_char_t c);
extern int pod_scan_simple(pod_stream *stream, pod_char_t c);
extern int pod_scan_quoted(pod_stream *stream, pod_char_t c);
extern int pod_scan_blurb(pod_stream *stream, pod_char_t c);
extern int pod_scan_blurb_pre_size(pod_stream *stream, pod_char_t c);
extern int pod_scan_blurb_size(pod_stream *stream, pod_char_t c);
extern int pod_scan_blurb_post_size(pod_stream *stream, pod_char_t c);
extern int pod_scan_append_to_buffer(pod_string *buffer, pod_char_t c);




#endif /* INCLUDE_POD_SCAN_H */
