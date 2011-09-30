#ifndef INCLUDE_POD_STREAM_H
#define INCLUDE_POD_STREAM_H

#include "pod_char.h"
#include "pod_object.h"
#include "pod_string.h"
#include "pod_list.h"
#include "pod_mapping.h"
#include "pod_map.h"
#include "pod_stream_state.h"
#include "pod_stream_token.h"



typedef void pod_stream_process_pod(pod_object *object);
typedef int pod_stream_handle_write_error(pod_stream *stream, int error);


// Next is pod_stream.  This is a struct that holds information about what to
// accept from a data source or send to a data sink.

struct pod_stream;
typedef struct pod_stream pod_stream;

struct pod_stream {
    pod_string *name;
    pod_list *stack;
    int allow_blurbs;
    int c;

    int total_characters;   // total characters (pod_char_t) received, from
                            //   Pod's POV
    int total_warnings;     // number of warnings since first started reading
                            //   (negative means warnings are unlimited)
    int max_warnings;       // maximum number of warnings before Pod ignores
                            //   further input.
    int position;           // current character within line, starting at zero
    int line;               // current line number, starting at zero

    pod_stream_state state;
    int string_size;
    int have_concat;
    int warn_string_size;
    int max_string_size;
    pod_string *buffer;
    pod_string *current_string;
    pod_stream_process_pod *process_pod;

    int escape_number;
    int escape_size;
    int escape_max_size;    // = sizeof(pod_char_t) * 8;
    pod_char_t escape_value;

    pod_object *result_pod;

    size_t blurb_size;
    size_t max_blurb_size;   // maximum blurb size

    int max_pod_size;     // maximum pod size
    int max_pod_depth;    // maximum pod depth
//no    int pod_timeout;      // maximum wait time reading within a pod
//no    int inter_timeout;    // maximum wait time between pods (0 = indefinite)
    int pod_errors;       // number of errors since pod beginning
    int max_pod_errors;   // max number of errors since pod beginning

    int fd;

    char  *r_buffer;        // allocate at initialize time
    size_t r_head;          // initialize to 0
    ? size_t r_mask;          // initialize from r_size, eg. 128 it would be 7f
    size_t r_size;          // default to 128 (arbitrary)
    size_t r_tail;          // initialize to 0

    char  *w_buffer;        // allocate at initialize time
    size_t w_head;          // initialize to 0
    size_t w_mask;          // initialize from w_size, eg. 128 it would be 7f
    size_t w_size;          // default to 128 (arbitrary)
    size_t w_tail;          // initialize to 0
    pod_stream_handle_write_error w_handler;    // default to default write
                                                // error handler

//    work buffer (for strings) (two times max string size?)
//    link endian-ness: I don't think endianess is Pod's problem, except maybe
//        in blurbs.
//    how do you negotiate endian-ness?  Is that even meaningful?
//    how do you find out max sizes?
};



    // Constructor, destructor, and initializer
    //      Allocate the memory, set the struct members, call the
    //      initializer.  Use the stream.  Call the destructor.

extern pod_stream *pod_stream_create();
extern pod_stream *pod_stream_initialize(pod_stream *stream);
extern void pod_stream_destroy(void *target);


    // Other pod_stream-related functions

extern int pod_stream_add_char(pod_stream *stream, pod_char_t c);
extern int pod_stream_add_token(pod_stream *stream, pod_stream_token token);
extern int pod_stream_end(pod_stream *stream);
extern void pod_stream_log(pod_stream *stream, int msg, char *fname, int line);



#endif /* INCLUDE_POD_STREAM_H */
