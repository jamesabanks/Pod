#include <pod_stream.h>



void pod_stream_add_char(
    pod_stream *stream,
    pod_char_t c,
    pod_object **object,
    int *error
)
{
    if (stream->state & lexer_escape_mask) {
        (*error) = lexer_handle_escape(c, &replace, &replacement, &finished);
        if (replace) {
            buffer->add_char(replacement);
        }
        if (finished) {
            state = next_state & lexer_state_mask;
        }
    } else {
        switch (stream->state) {
            case lexer_initial:
                lexer_handle_initial(stream, c, &next_state);
                break;
            case lexer_string:
                lexer_handle_string(stream, c, &finish_string, &send_token, &next_state);
                break;
            case lexer_quoted:
                lexer_handle_quoted(stream, c, &finish_string, &send_token, &next_state);
            case lexer_blurb:
                lexer_handle_blurb(stream, c, &next_state);
                state = next_state;
            default:
                error;
        }
    }
    if (finish_string) {
        send_parse_token(string);
    }
    if (send_token) {
        send_parse_token(send_token);
    }
    state = next_state;
}

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
