/******************************************************************** 
 *  pod_stream.c
 *  Copyright (c) 2011-2012, James A. Banks
 *  All rights reserved.
 *  See file LICENSE for details.
 ********************************************************************/

#include <assert.h>
#include <stdlib.h>
#include "pod_stream.h"
#include "pod_scan.h"
#include "pod_boolean.h"
#include "pod_log.h"



    // ceiling
    //
    // Round up the provided number to the power of two greater than or equal
    // to a number.  I'm assuming that size_t is unsigned.  If the number is 0,
    // or if size_t isn't unsigned and the number is negative, this routine
    // returns 1.
    //
    // Returns:
    //    The power of two greater than or equal to the number.  

static inline size_t ceiling(size_t n)
{
    size_t c;

    c = 1;
    while (n > c) {
        c >>= 1;
    }

    return c;
}



    // pod_stream_create
    //

pod_stream *pod_stream_create(void)
{
    pod_stream *stream;

    stream = (pod_stream *) malloc(sizeof(pod_stream));
    if (stream != NULL) {
        stream->o.n.previous = NULL;
        stream->o.n.next = NULL;
        stream->o.type = POD_STREAM_TYPE;
        stream->o.destroy = pod_stream_destroy;
        
        stream->name = NULL;            // needs to be set to a useful name;
        stream->total_characters = 0;   // total characters received
        stream->total_warnings = 0;     // warnings since first started reading
        stream->max_warnings = 1000;    // maximum number of warnings allowed
        stream->position = 0;           // current character within line (0)
        stream->line = 0;               // current line number, starting at zero

    // int string_size;
    // int warn_string_size;
    // int max_string_size;
    // pod_string *current_string;

        stream->escape = 0;
        stream->escape_size = 0;
        stream->escape_max_size = 16;   // = sizeof(pod_char_t) * 2;

        stream->result_pod = NULL;      // initialize to NULL;

        stream->allow_blurbs = false;
        stream->blurb_size = 0;
        stream->max_blurb_size = 100;         // maximum blurb size

    // int max_pod_size;       // maximum pod size
    // int max_pod_depth;      // maximum pod depth
//no    int pod_timeout;      // maximum wait time reading within a pod
//no    int inter_timeout;    // maximum wait time between pods (0 = indefinite)
    // int pod_errors;         // number of errors since pod beginning
    // int max_pod_errors;     // max number of errors since pod beginning

        stream->fd = -1;        // this MUST be provided/initialized!

        stream->r_size = 128;   // create to 128 (arbitrary).  Will be rounded
                                // up to the next power of 2.
    // char  *r_buffer;        // allocate at initialize time
        stream->r_head = 0;     // initialize to 0
        stream->r_tail = 0;     // initialize to 0

        stream->w_size = 128;   // create to 128 (arbitrary).  Will be rounded
                                // up to the next power of 2.
    // size_t w_mask;          // initialize from w_size, eg. 128 it would be 7f
    // char  *w_buffer;        // allocate at initialize time
        stream->w_head = 0;     // initialize to 0
        stream->w_tail = 0;     // initialize to 0
        stream->w_stack = pod_list_create();    // allocate at create time
        stream->w_object = NULL;                // does't need allocation

        stream->have_concat = false;            // create to false

        stream->s_state = POD_STATE_START;      // create to POD_STATE_START
        stream->s_buffer_size = 10;             // create to 20 (arbitrary)
    // pod_string s_buffer;    // allocate at initialize time
        stream->s_stack = pod_list_create();    // allocate at create time
    }

    return stream;
}



    // pod_stream_initialize

pod_stream *pod_stream_initialize(pod_stream *stream)
{
    assert(stream != NULL);
    assert(stream->fd >= 0);

    if (stream->name == NULL) {
        // make a name from the fd
    }
    stream->r_size = ceiling(stream->r_size);
    stream->r_buffer = malloc(stream->r_size);
    stream->w_size = ceiling(stream->w_size);
    stream->w_buffer = malloc(stream->w_size);
    stream->s_buffer = pod_string_create(stream->s_buffer_size, 0);

    return stream;
}



    // pod_stream_destroy

void pod_stream_destroy(void *target)
{
    pod_stream *stream;

    stream = (pod_stream *) target;
    pod_string_destroy(stream->s_buffer);
    free(stream->w_buffer);
    free(stream->r_buffer);
    // ? stream->w_object
    pod_list_destroy(stream->w_stack);
    pod_string_destroy(stream->name);

    free(stream);
}



    // pod_stream_log

void pod_stream_log(pod_stream *stream, int message, char *file_name, int line)
{
    // Example:
    //
    // Pod message 127: Got illegal character ')'
    //   stream: std_out
    //   at character xxxx (line xxx, column xx)
    //   message from pod_stream.c, line xxx
}
