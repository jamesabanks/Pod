#include <stdlib.h>
#include "pod_stream.h"
#include "pod_scan.h"
#include "pod_boolean.h"
#include "pod_log.h"



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
    // pod_string *name;
    // int allow_blurbs;

    // int total_characters;   // total characters (pod_char_t) received, from
                            //   Pod's POV
    // int total_warnings;     // number of warnings since first started reading
                            //   (negative means warnings are unlimited)
    // int max_warnings;       // maximum number of warnings before Pod ignores
                            //   further input.
    // int position;           // current character within line, starting at zero
    // int line;               // current line number, starting at zero

    // int string_size;
    // int warn_string_size;
    // int max_string_size;
    // pod_string *current_string;

    // int escape_size;
    // int escape_max_size;    // = sizeof(pod_char_t) * 2;
    // pod_char_t escape_value;

    // pod_object *result_pod; //

    // size_t blurb_size;
    // size_t max_blurb_size;  // maximum blurb size

    // int max_pod_size;       // maximum pod size
    // int max_pod_depth;      // maximum pod depth
//no    int pod_timeout;      // maximum wait time reading within a pod
//no    int inter_timeout;    // maximum wait time between pods (0 = indefinite)
    // int pod_errors;         // number of errors since pod beginning
    // int max_pod_errors;     // max number of errors since pod beginning

    // int fd;

    // char  *r_buffer;        // allocate at initialize time
    // size_t r_head;          // initialize to 0
    // size_t r_mask;          // ? initialize from r_size, eg. 128 it would be 7f
    // size_t r_size;          // default to 128 (arbitrary)
    // size_t r_tail;          // initialize to 0

    // char  *w_buffer;        // allocate at initialize time
    // size_t w_head;          // initialize to 0
    // size_t w_mask;          // initialize from w_size, eg. 128 it would be 7f
    // size_t w_size;          // default to 128 (arbitrary)
    // size_t w_tail;          // initialize to 0
    // pod_list *w_stack;
    // pod_object *w_object;

    // int have_concat;        // initialize to false
    // pod_scan_state s_state; // scanner state, init to POD_STATE_START
    // pod_string *s_buffer;   // allocate at create time
    // pod_list *s_stack;      // scanner stack, allocate at create time
    }

    return stream;
}



    // pod_stream_initialize

pod_stream *pod_stream_initialize(pod_stream *stream)
{
}



    // pod_stream_destroy

void pod_stream_destroy(void *target)
{
    pod_stream *stream;

    stream = (pod_stream *) target;
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
