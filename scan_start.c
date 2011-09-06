#include <scanner.h>



    // scan_start
    // 
    // At this point, Pod has nothing.
    //
    // Returns:
    //      int     The error id of any problem that occurred (0 = no error)

int scan_start(pod_stream *stream, pod_char_t c, pod_object **object)
{
    int have_string;
    int warning;

    have_string = ! pod_stream_is_empty(stream->buffer);
    warning = 0;
    switch (c) {
        case '\t': // Do nothing.
        case '\n':
        case '\r':
        case ' ':
            break;
        case '"':  // Start a quoted string
            if ((!stream->have_concat) && (have_string)) {
                add_token(stream, stream_string, object);
            }
            stream->state = stream_quoted;
            break;
        case '+':
            stream->have_concat = true;
            break;
        case '<':
            if (have_string) { add_token(stream, stream_string, object); }
            add_token(stream, stream_begin_map, object);
            break;
        case '=':
            if (have_string) { add_token(stream, stream_string, object); }
            add_token(stream, stream_equals, object);
            break;
        case '>':
            if (have_string) { add_token(stream, stream_string, object); }
            add_token(stream, stream_end_map, object);
            break;
        case '[':
            if (have_string) { add_token(stream, stream_string, object); }
            add_token(stream, stream_begin_blurb, object);
            break;
        case '\\':
            if ((!stream->have_concat) && (have_string)) {
                add_token(stream, stream_string, object);
            }
            state = stream_string_escape;
            break;
        case ']':
            if (have_string) { add_token(stream, stream_string, object); }
            add_token(stream, stream_end_blurb);
            // TODO Shouldn't get ']' in start state.
            warning = 1;
            break;
        case '{':
            if (have_string) { add_token(stream, stream_string, object); }
            add_token(stream, stream_begin_list, object);
            break;
        case '}':
            if (have_string) { add_token(stream, stream_string, object); }
            add_token(stream, stream_end_list, object);
            break;
        case '':
            if (have_string) { add_token(stream, stream_string, object); }
            add_token(stream, stream_pod_sync, object);
            break;
        default:
            if (c < 32) {
                // TODO warn, illegal char (?).  What about non-printing
                // characters that are above 31?
                warning = 1;
            } else {
                pod_string_add_char(stream->buffer, c);
                state = stream_simple;
            }
            break;
    }

    return warning;
}
