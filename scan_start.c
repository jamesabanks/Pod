#include "scan.h"



    // scan_start
    // 
    // At this point, Pod has nothing.
    //
    // Returns:
    //      int     The error id of any problem that occurred (0 = no error)

int scan_start(pod_stream *stream, pod_char_t c)
{
    int have_string;
    int warning;

    have_string = ! pod_string_is_empty(stream->buffer);
    warning = 0;
    switch (c) {
        case '\t': // Do nothing.
        case '\n':
        case '\r':
        case ' ':
            break;
        case '"':  // Start a quoted string
            if ((!stream->have_concat) && (have_string)) {
                pod_stream_add_token(stream, pod_token_string);
            }
            stream->state = pod_quoted;
            break;
        case '+':
            stream->have_concat = true;
            break;
        case '<':
            if (have_string) { pod_stream_add_token(stream, pod_token_string); }
            pod_stream_add_token(stream, pod_token_begin_map);
            break;
        case '=':
            if (have_string) { pod_stream_add_token(stream, pod_token_string); }
            pod_stream_add_token(stream, pod_token_equals);
            break;
        case '>':
            if (have_string) { pod_stream_add_token(stream, pod_token_string); }
            pod_stream_add_token(stream, pod_token_end_map);
            break;
        case '[':
            if (have_string) { pod_stream_add_token(stream, pod_token_string); }
            pod_stream_add_token(stream, pod_token_begin_blurb);
            break;
        case '\\':
            if ((!stream->have_concat) && (have_string)) {
                pod_stream_add_token(stream, pod_token_string);
            }
            stream->state = pod_simple_escape;
            break;
        case ']':
            if (have_string) { pod_stream_add_token(stream, pod_token_string); }
            pod_stream_add_token(stream, pod_token_end_blurb);
            // TODO Shouldn't get ']' in start state.
            warning = 1;
            break;
        case '{':
            if (have_string) { pod_stream_add_token(stream, pod_token_string); }
            pod_stream_add_token(stream, pod_token_begin_list);
            break;
        case '}':
            if (have_string) { pod_stream_add_token(stream, pod_token_string); }
            pod_stream_add_token(stream, pod_token_end_list);
            break;
        case '':
            if (have_string) { pod_stream_add_token(stream, pod_token_string); }
            pod_stream_add_token(stream, pod_token_pod_sync);
            break;
        default:
            if (c < 32) {
                // TODO warn, illegal char (?).  What about non-printing
                // characters that are above 31?
                warning = 1;
            } else {
                pod_stream_add_char(stream, c);
                stream->state = pod_simple;
            }
            break;
    }

    return warning;
}
