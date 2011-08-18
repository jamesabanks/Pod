#ifndef INCLUDE_POD_STREAM_H
#define INCLUDE_POD_STREAM_H

#include "pod_char.h"



enum stream_state {
    stream_empty
    stream_in_ordered
    stream_in_unordered
    stream_in_string
    stream_in_quoted
    stream_in_blurb
    stream_escaped
}

enum pod_tokens {
    start_ordered
    end_ordered
    start_unordered
    end_unordered
    string
    mapping
}


typedef int pod_token_t;



// Next is pod_stream.  This is a struct that holds information about what to
// accept from a data source or send to a data sink.

struct pod_stream;

typedef struct pod_stream pod_stream;


struct pod_stream {
    int state;
    pod_string *buffer;
    int max_string_size;  // maximum string size
    int warn_string_size;

    int max_pod_size;     // maximum pod size
    int max_blurb_size;   // maximum blurb size
    int max_pod_depth;    // maximum pod depth
    int total_characters; // total characters received, from Pod's POV
    int line_num;         // current line number, starting at zero
    int char_num;         // current character within line, starting at zero
    int pod_timeout;      // maximum wait time reading within a pod
    int inter_timeout;    // maximum wait time between pods (0 = indefinite)
    int total_errors;     // number of errors since first started reading
    int max_total_errors; // max number of errors since first started reading
    int pod_errors;       // number of errors since pod beginning
    int max_pod_errors;   // max number of errors since pod beginning
    work buffer (for strings) (two times max string size?)
    link endian-ness: I don't think endianess is Pod's problem, except maybe in
        blurbs.
    how do you negotiate endian-ness?  Is that even meaningful?
    how do you find out max sizes?
};



    // Constructor, destructor, and initializer
    //      Allocate the memory, set the struct members, call the
    //      initializer.  Use the stream.  Call the destructor.

extern pod_stream *pod_stream_create();
extern pod_stream *pod_stream_initialize(pod_stream *stream);
extern void pod_stream_destroy(void *target);


    // Other pod_stream-related functions

extern void pod_stream_add_char(
    pod_stream *stream,
    pod_char_t c,
    pod_object **object,
    int error
);
extern void pod_stream_add_token(
    pod_stream *stream,
    pod_token_t token,
    pod_object **object,
    int error
);
extern void pod_stream_end(
    pod_stream *stream,
    pod_object **object,
    int error
);




#endif /* INCLUDE_POD_STREAM_H */
