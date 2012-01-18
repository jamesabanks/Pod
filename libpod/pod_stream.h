#ifndef INCLUDE_POD_STREAM_H
#define INCLUDE_POD_STREAM_H

/******************************************************************** 
 *  pod_stream.h
 *  Copyright (c) 2011-2012, James A. Banks
 *  All rights reserved.
 *  See file LICENSE for details.
 ********************************************************************/

#include "pod_char.h"
#include "pod_object.h"
#include "pod_string.h"
#include "pod_list.h"
#include "pod_mapping.h"
#include "pod_scan_state.h"
#include "pod_scan_token.h"



#define POD_STREAM_TYPE 0x67



// Next is pod_stream.  This is a struct that holds information about what to
// accept from a data source or send to a data sink.

typedef struct pod_stream {
    pod_object o;
    pod_string *name;
    int allow_blurbs;

    int total_characters;   // total characters (pod_char_t) received, from
                            //   Pod's POV
    int total_warnings;     // number of warnings since first started reading
                            //   (negative means warnings are unlimited)
    int max_warnings;       // maximum number of warnings before Pod ignores
                            //   further input.
    int position;           // current character within line, starting at zero
    int line;               // current line number, starting at zero

    //int string_size;
    //int warn_string_size;
    //int max_string_size;
    //pod_string *current_string;

    pod_char_t escape;
    int escape_size;
    int escape_max_size;    // In an escape, the max number of digits to allow.
                            //   A resonable default is POD_CHAR_BITS / 4,
                            //   because a hex digit is 4 bits.

    pod_object *result_pod; // initialize to NULL

    size_t blurb_size;
    size_t max_blurb_size;  // maximum blurb size

    //int max_pod_size;       // maximum pod size
    //int max_pod_depth;      // maximum pod depth
//no    int pod_timeout;      // maximum wait time reading within a pod
//no    int inter_timeout;    // maximum wait time between pods (0 = indefinite)
    //int pod_errors;         // number of errors since pod beginning
    //int max_pod_errors;     // max number of errors since pod beginning

    int fd;

    char  *r_buffer;        // allocate at initialize time
    size_t r_head;          // initialize to 0
    // remove this ??? size_t r_mask;          // ? initialize from r_size, eg. 128 it would be 7f
    size_t r_size;          // default to 128 (arbitrary)
    size_t r_tail;          // initialize to 0

    char  *w_buffer;        // allocate at initialize time
    size_t w_head;          // initialize to 0
    size_t w_mask;          // initialize from w_size, eg. 128 it would be 7f
    size_t w_size;          // default to 128 (arbitrary)
    size_t w_tail;          // initialize to 0
    pod_list *w_stack;
    pod_object *w_object;

    int have_concat;        // initialize to false
    pod_scan_state s_state; // scanner state, init to POD_STATE_START
    int s_buffer_size;      // initialize to, say, 10, used to create s_buffer
    pod_string *s_buffer;   // allocate at initialize time
    pod_list *s_stack;      // scanner stack, allocate at create time

} pod_stream;



    // Constructor, destructor, and initializer
    //      Allocate the memory, set the struct members, call the
    //      initializer.  Use the stream.  Call the destructor.

extern pod_stream *pod_stream_create();
extern pod_stream *pod_stream_initialize(pod_stream *stream);
extern void pod_stream_destroy(void *target);


    // Other pod_stream-related functions

extern int pod_stream_read(pod_stream *stream, pod_object **object, int *os_er);
extern void pod_stream_read_reset(pod_stream *stream);
extern int pod_stream_write(pod_stream *stream, pod_object *object, int *os_er);
extern void pod_stream_write_reset(pod_stream *stream);
extern int pod_stream_add_char(pod_stream *stream, pod_char_t c);
extern int pod_stream_add_token(pod_stream *stream, pod_scan_token token);
// to be written:
extern int pod_stream_end(pod_stream *stream);
extern void pod_stream_log(pod_stream *stream, int msg, char *fname, int line);
// extern int pod_stream_write_sync

#endif /* INCLUDE_POD_STREAM_H */
