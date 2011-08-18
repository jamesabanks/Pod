#include <pod_stream.h>



void pod_stream_add_char(
    pod_stream *stream,
    pod_char_t c,
    pod_object **object,
    int error
)
{
    what do we do given c and the current state?
    what is the next state based on c?
    pass on a token?
    state = new_state?

    if ((stream->state == stream_in_string || (stream->state == stream_in_quoted)) {
        (put a name on the stream for better error tracking?)
        if we're over max_string_size, return error
        if we're over warn_string_size, emit warning
            
    }
    switch (stream->state) {
        case stream_empty:
            switch (c) {
                case '{':
                    // if we have a string in the buffer, call add_token for it.
                    // don't add to buffer, buffer should be empty
                    // add_token(start_ordered)
                    // new_state = stream_in_ordered
                    break;
                case '}':
                    // if we have a string in the buffer, call add_token for it.
                    // don't add to buffer, buffer should be empty  
                    // add_token(end_ordered), should get warning
                    // new_state = stream_empty (no change)
                    break;
                case '"':
                    new_state = stream_in_quoted;
                case '<':
                case '>':
                (?) case ',':
                        
            }
        case stream_in_ordered:
        default:
    }
}